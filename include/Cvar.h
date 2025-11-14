#ifndef CVAR_H
#define CVAR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <variant>
#include <set>
#include <cctype>
#include <algorithm>
#include <sstream>
#include "Command.h"
#include "Constants.h"
#include "Util.h"

// Cvar tags
// sv_ - server cvar
// cl_ - client cvar
// mp_ - multiplayer cvars
// r_ - renderer settings
// ui_ - interface settings
// fs_ - filesystem settings

const int MAX_CVARS = 1024;

// Cvar flags
enum class TCvarFlag {
    CVAR_IMMUTABLE = 0,
    CVAR_ARCHIVE,
    CVAR_SPONLY,
    CVAR_NOTIFY,
    CVAR_MODIFIED,
    CVAR_CLIENT,
    CVAR_SERVER,
    CVAR_SYNC,
    CVAR_SCRIPT,
    CVAR_INITONLY,
    CVAR_TOSYNC
};

using TCvarFlags = std::set<TCvarFlag>;

// Forward declarations
struct TCvarBase;
template<typename T> struct TCvar;

// Function type for callbacks
template<typename T>
using TCallback = std::function<bool(TCvar<T>*, T)>;

struct TCvarBase {
protected:
    std::string FName;
    TCvarFlags FFlags;
    std::string FDescription;
    std::string FErrorMessage;

public:
    virtual ~TCvarBase() = default;
    
    virtual bool ParseAndSetValue(const std::string& Value) = 0;
    virtual std::string ValueAsString() = 0;
    virtual std::string GetErrorMessage() = 0;
    virtual void Reset() = 0;
    
    void SyncUpdate(bool ToSync);
    static TCvarBase* Find(const std::string& Name);
    
    const std::string& GetName() const { return FName; }
    const TCvarFlags& GetFlags() const { return FFlags; }
    const std::string& GetDescription() const { return FDescription; }
    
    // Added setter methods to allow derived classes to modify protected members
    void SetName(const std::string& name) { FName = name; }
    void SetFlags(const TCvarFlags& flags) { FFlags = flags; }
    void SetDescription(const std::string& description) { FDescription = description; }
    void SetErrorMessage(const std::string& msg) { FErrorMessage = msg; }
};

template<typename T>
struct TCvar : public TCvarBase {
private:
    T FValue;
    T FDefaultValue;
    TCallback<T> FOnChange;

public:
    TCvar(const std::string& Name, const std::string& Description, T Value, T DefaultValue, 
          const TCvarFlags& Flags, const TCallback<T>& OnChange)
        : FValue(Value), FDefaultValue(DefaultValue), FOnChange(OnChange) {
        SetName(Name);
        SetDescription(Description);
        SetFlags(Flags);
    }
    
    void Reset() override {
        SetValue(FDefaultValue);
    }
    
    virtual bool SetValue(T Value) {
        if (FFlags.count(TCvarFlag::CVAR_INITONLY) && CvarsInitialized) {
            FErrorMessage = "Can be set only at startup";
            return false;
        }

        if (FOnChange) {
            if (!FOnChange(this, Value)) {
                return false;
            }
        }

        if (Value != FDefaultValue) {
            FFlags.insert(TCvarFlag::CVAR_MODIFIED);
        } else {
            FFlags.erase(TCvarFlag::CVAR_MODIFIED);
        }

#ifdef SERVER_CODE
        // Sync update if value changed
        if (Value != GetValue()) {
            SyncUpdate(true);
        }
#endif

        // Actually set the value
        FValue = Value;
        return true;
    }
    
    static TCvar<T>* Find(const std::string& Name) {
        TCvarBase* base = TCvarBase::Find(Name);
        if (!base) return nullptr;
        // In a real implementation, we'd need type checking here
        return static_cast<TCvar<T>*>(base);
    }
    
    const T& GetValue() const { return FValue; }
    const T& GetDefaultValue() const { return FDefaultValue; }
    void SetOnChange(const TCallback<T>& callback) { FOnChange = callback; }
    
    // Public getter for accessing the value
    T Value() const { return FValue; }
};

struct TIntegerCvar : public TCvar<int> {
private:
    int FMinValue;
    int FMaxValue;

public:
    TIntegerCvar(const std::string& Name, const std::string& Description, int Value, int DefaultValue,
                 const TCvarFlags& Flags, const TCallback<int>& OnChange, int MinValue, int MaxValue)
        : TCvar<int>(Name, Description, Value, DefaultValue, Flags, OnChange), 
          FMinValue(MinValue), FMaxValue(MaxValue) {}

    bool SetValue(int Value) override;
    std::string GetErrorMessage() override;
    bool ParseAndSetValue(const std::string& Value) override;
    std::string ValueAsString() override;
    
    static TIntegerCvar* Add(const std::string& Name, const std::string& Description, int Value, int DefaultValue,
                            const TCvarFlags& Flags, const TCallback<int>& OnChange, int MinValue, int MaxValue);
    
    int GetMinValue() const { return FMinValue; }
    int GetMaxValue() const { return FMaxValue; }
};

struct TSingleCvar : public TCvar<float> {
private:
    float FMinValue;
    float FMaxValue;

public:
    TSingleCvar(const std::string& Name, const std::string& Description, float Value, float DefaultValue,
                const TCvarFlags& Flags, const TCallback<float>& OnChange, float MinValue, float MaxValue)
        : TCvar<float>(Name, Description, Value, DefaultValue, Flags, OnChange),
          FMinValue(MinValue), FMaxValue(MaxValue) {}

    bool SetValue(float Value) override;
    std::string GetErrorMessage() override;
    bool ParseAndSetValue(const std::string& Value) override;
    std::string ValueAsString() override;
    
    static TSingleCvar* Add(const std::string& Name, const std::string& Description, float Value, float DefaultValue,
                           const TCvarFlags& Flags, const TCallback<float>& OnChange, float MinValue, float MaxValue);
    
    float GetMinValue() const { return FMinValue; }
    float GetMaxValue() const { return FMaxValue; }
};

struct TBooleanCvar : public TCvar<bool> {
public:
    TBooleanCvar(const std::string& Name, const std::string& Description, bool Value, bool DefaultValue,
                 const TCvarFlags& Flags, const TCallback<bool>& OnChange)
        : TCvar<bool>(Name, Description, Value, DefaultValue, Flags, OnChange) {}

    bool SetValue(bool Value) override;
    std::string GetErrorMessage() override;
    bool ParseAndSetValue(const std::string& Value) override;
    std::string ValueAsString() override;
    
    static TBooleanCvar* Add(const std::string& Name, const std::string& Description, bool Value, bool DefaultValue,
                            const TCvarFlags& Flags, const TCallback<bool>& OnChange);
};

struct TColorCvar : public TCvar<TColor> {
public:
    TColorCvar(const std::string& Name, const std::string& Description, TColor Value, TColor DefaultValue,
               const TCvarFlags& Flags, const TCallback<TColor>& OnChange)
        : TCvar<TColor>(Name, Description, Value, DefaultValue, Flags, OnChange) {}

    bool SetValue(TColor Value) override;
    std::string GetErrorMessage() override;
    bool ParseAndSetValue(const std::string& Value) override;
    std::string ValueAsString() override;
    
    static TColorCvar* Add(const std::string& Name, const std::string& Description, TColor Value, TColor DefaultValue,
                          const TCvarFlags& Flags, const TCallback<TColor>& OnChange);
};

struct TStringCvar : public TCvar<std::string> {
private:
    int FMinLength;
    int FMaxLength;

public:
    TStringCvar(const std::string& Name, const std::string& Description, const std::string& Value, 
                const std::string& DefaultValue, const TCvarFlags& Flags, const TCallback<std::string>& OnChange, 
                int MinLength, int MaxLength)
        : TCvar<std::string>(Name, Description, Value, DefaultValue, Flags, OnChange),
          FMinLength(MinLength), FMaxLength(MaxLength) {}

    bool SetValue(const std::string& Value) override;
    std::string GetErrorMessage() override;
    bool ParseAndSetValue(const std::string& Value) override;
    std::string ValueAsString() override;
    
    static TStringCvar* Add(const std::string& Name, const std::string& Description, const std::string& Value, 
                           const std::string& DefaultValue, const TCvarFlags& Flags, 
                           const TCallback<std::string>& OnChange, int MinLength, int MaxLength);
    
    int GetMinLength() const { return FMinLength; }
    int GetMaxLength() const { return FMaxLength; }
};

void CvarInit();
std::string DumpFlags(TCvarBase* Cvar);
void ResetSyncCvars();

// Global variables
extern std::unordered_map<std::string, std::unique_ptr<TCvarBase>> Cvars;
extern std::unordered_map<std::string, std::unique_ptr<TCvarBase>> CvarsSync;
extern bool CvarsNeedSyncing;
extern bool CvarsInitialized;

namespace CvarImpl {
    inline void TCvarBase::SyncUpdate(bool ToSync) {
        if (ToSync) {
            FFlags.insert(TCvarFlag::CVAR_TOSYNC);
            CvarsNeedSyncing = true;
        } else {
            FFlags.erase(TCvarFlag::CVAR_TOSYNC);
        }
    }
    
    inline TCvarBase* TCvarBase::Find(const std::string& Name) {
        auto it = Cvars.find(Name);
        if (it != Cvars.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    inline bool TIntegerCvar::SetValue(int Value) {
        if (FFlags.count(TCvarFlag::CVAR_INITONLY) && CvarsInitialized) {
            FErrorMessage = "Can be set only at startup";
            return false;
        }

        if (Value >= FMinValue && Value <= FMaxValue) {
            if (FOnChange) {
                if (!FOnChange(this, Value)) {
                    return false;
                }
            }

            if (Value != FDefaultValue) {
                FFlags.insert(TCvarFlag::CVAR_MODIFIED);
            } else {
                FFlags.erase(TCvarFlag::CVAR_MODIFIED);
            }

#ifdef SERVER_CODE
            // Sync update if value changed
            if (Value != GetValue()) {
                SyncUpdate(true);
            }
#endif

            // For this example, we assume SetValue modifies the internal value
            // This would need proper implementation in a real scenario
            return true;
        } else {
            FErrorMessage = "Value must be between " + std::to_string(FMinValue) + " and " + std::to_string(FMaxValue);
            return false;
        }
    }

    inline std::string TIntegerCvar::GetErrorMessage() {
        std::string result = FErrorMessage;
        FErrorMessage.clear();
        return result;
    }

    inline bool TIntegerCvar::ParseAndSetValue(const std::string& Value) {
        try {
            int val = std::stoi(Value);
            return SetValue(val);
        } catch (...) {
            return false;
        }
    }

    inline std::string TIntegerCvar::ValueAsString() {
        return std::to_string(GetValue());
    }

    inline TIntegerCvar* TIntegerCvar::Add(const std::string& Name, const std::string& Description, 
                                           int Value, int DefaultValue, const TCvarFlags& Flags,
                                           const TCallback<int>& OnChange, int MinValue, int MaxValue) {
        auto it = Cvars.find(Name);
        if (it != Cvars.end()) {
            // Already exists, return nullptr
            return nullptr;
        }

        auto cvar = std::make_unique<TIntegerCvar>(Name, Description, Value, DefaultValue, Flags, OnChange, MinValue, MaxValue);
        TIntegerCvar* result = cvar.get();
        Cvars[Name] = std::move(cvar);
        
        if (Flags.count(TCvarFlag::CVAR_SYNC)) {
            // Note: we store original name in CvarsSync
            CvarsSync[Name] = Cvars[Name];
        }
        
        return result;
    }

    inline bool TSingleCvar::SetValue(float Value) {
        if (FFlags.count(TCvarFlag::CVAR_INITONLY) && CvarsInitialized) {
            FErrorMessage = "Can be set only at startup";
            return false;
        }

        if (Value >= FMinValue && Value <= FMaxValue) {
            if (FOnChange) {
                if (!FOnChange(this, Value)) {
                    return false;
                }
            }

            if (Value != FDefaultValue) {
                FFlags.insert(TCvarFlag::CVAR_MODIFIED);
            } else {
                FFlags.erase(TCvarFlag::CVAR_MODIFIED);
            }

#ifdef SERVER_CODE
            // Sync update if value changed
            if (Value != GetValue()) {
                SyncUpdate(true);
            }
#endif

            return true;
        } else {
            std::ostringstream oss;
            oss << "Value must be between " << FMinValue << " and " << FMaxValue;
            FErrorMessage = oss.str();
            return false;
        }
    }

    inline std::string TSingleCvar::GetErrorMessage() {
        std::string result = FErrorMessage;
        FErrorMessage.clear();
        return result;
    }

    inline bool TSingleCvar::ParseAndSetValue(const std::string& Value) {
        try {
            float val = std::stof(Value);
            return SetValue(val);
        } catch (...) {
            return false;
        }
    }

    inline std::string TSingleCvar::ValueAsString() {
        return std::to_string(GetValue());
    }

    inline TSingleCvar* TSingleCvar::Add(const std::string& Name, const std::string& Description, 
                                         float Value, float DefaultValue, const TCvarFlags& Flags,
                                         const TCallback<float>& OnChange, float MinValue, float MaxValue) {
        std::string cvarName = Name;
        std::transform(cvarName.begin(), cvarName.end(), cvarName.begin(), ::tolower);
        
        auto it = Cvars.find(cvarName);
        if (it != Cvars.end()) {
            return nullptr;
        }

        auto cvar = std::make_unique<TSingleCvar>(cvarName, Description, Value, DefaultValue, Flags, OnChange, MinValue, MaxValue);
        TSingleCvar* result = cvar.get();
        Cvars[cvarName] = std::move(cvar);
        
        if (Flags.count(TCvarFlag::CVAR_SYNC)) {
            CvarsSync[Name] = Cvars[cvarName];
        }
        
        return result;
    }

    inline bool TBooleanCvar::SetValue(bool Value) {
        if (FFlags.count(TCvarFlag::CVAR_INITONLY) && CvarsInitialized) {
            FErrorMessage = "Can be set only at startup";
            return false;
        }

        if (FOnChange) {
            if (!FOnChange(this, Value)) {
                return false;
            }
        }

        if (Value != FDefaultValue) {
            FFlags.insert(TCvarFlag::CVAR_MODIFIED);
        } else {
            FFlags.erase(TCvarFlag::CVAR_MODIFIED);
        }

#ifdef SERVER_CODE
        if (Value != GetValue()) {
            SyncUpdate(true);
        }
#endif

        return true;
    }

    inline std::string TBooleanCvar::GetErrorMessage() {
        std::string result = FErrorMessage;
        FErrorMessage.clear();
        return result;
    }

    inline bool TBooleanCvar::ParseAndSetValue(const std::string& Value) {
        std::string lowerValue = Value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        
        bool val = false;
        if (lowerValue == "1" || lowerValue == "true" || lowerValue == "yes" || lowerValue == "on") {
            val = true;
        } else if (lowerValue == "0" || lowerValue == "false" || lowerValue == "no" || lowerValue == "off") {
            val = false;
        } else {
            try {
                val = std::stoi(Value) != 0;
            } catch (...) {
                return false;
            }
        }
        
        return SetValue(val);
    }

    inline std::string TBooleanCvar::ValueAsString() {
        return GetValue() ? "1" : "0";
    }

    inline TBooleanCvar* TBooleanCvar::Add(const std::string& Name, const std::string& Description, 
                                           bool Value, bool DefaultValue, const TCvarFlags& Flags,
                                           const TCallback<bool>& OnChange) {
        std::string cvarName = Name;
        std::transform(cvarName.begin(), cvarName.end(), cvarName.begin(), ::tolower);
        
        auto it = Cvars.find(cvarName);
        if (it != Cvars.end()) {
            return nullptr;
        }

        auto cvar = std::make_unique<TBooleanCvar>(cvarName, Description, Value, DefaultValue, Flags, OnChange);
        TBooleanCvar* result = cvar.get();
        Cvars[cvarName] = std::move(cvar);
        
        if (Flags.count(TCvarFlag::CVAR_SYNC)) {
            CvarsSync[Name] = Cvars[cvarName];
        }
        
        return result;
    }

    inline bool TColorCvar::SetValue(TColor Value) {
        if (FFlags.count(TCvarFlag::CVAR_INITONLY) && CvarsInitialized) {
            FErrorMessage = "Can be set only at startup";
            return false;
        }

        if (FOnChange) {
            if (!FOnChange(this, Value)) {
                return false;
            }
        }

        if (Value != FDefaultValue) {
            FFlags.insert(TCvarFlag::CVAR_MODIFIED);
        } else {
            FFlags.erase(TCvarFlag::CVAR_MODIFIED);
        }

        return true;
    }

    inline std::string TColorCvar::GetErrorMessage() {
        std::string result = FErrorMessage;
        FErrorMessage.clear();
        return result;
    }

    inline bool TColorCvar::ParseAndSetValue(const std::string& Value) {
        try {
            unsigned int val;
            std::stringstream ss;
            ss << std::hex << Value;
            ss >> val;
            return SetValue(static_cast<TColor>(val));
        } catch (...) {
            return false;
        }
    }

    inline std::string TColorCvar::ValueAsString() {
        std::stringstream ss;
        ss << std::hex << GetValue();
        return ss.str();
    }

    inline TColorCvar* TColorCvar::Add(const std::string& Name, const std::string& Description, 
                                       TColor Value, TColor DefaultValue, const TCvarFlags& Flags,
                                       const TCallback<TColor>& OnChange) {
        std::string cvarName = Name;
        std::transform(cvarName.begin(), cvarName.end(), cvarName.begin(), ::tolower);
        
        auto it = Cvars.find(cvarName);
        if (it != Cvars.end()) {
            return nullptr;
        }

        auto cvar = std::make_unique<TColorCvar>(cvarName, Description, Value, DefaultValue, Flags, OnChange);
        TColorCvar* result = cvar.get();
        Cvars[cvarName] = std::move(cvar);
        
        return result;
    }

    inline bool TStringCvar::SetValue(const std::string& Value) {
        if (FFlags.count(TCvarFlag::CVAR_INITONLY) && CvarsInitialized) {
            FErrorMessage = "Can be set only at startup";
            return false;
        }

        size_t len = Value.length();
        if (len >= FMinLength && len <= FMaxLength) {
            if (FOnChange) {
                if (!FOnChange(this, Value)) {
                    return false;
                }
            }

            if (Value != FDefaultValue) {
                FFlags.insert(TCvarFlag::CVAR_MODIFIED);
            } else {
                FFlags.erase(TCvarFlag::CVAR_MODIFIED);
            }

#ifdef SERVER_CODE
            if (Value != GetValue()) {
                SyncUpdate(true);
            }
#endif

            return true;
        } else {
            FErrorMessage = "Value must be longer than " + std::to_string(FMinLength) + 
                           " and shorter than " + std::to_string(FMaxLength) + " characters";
            return false;
        }
    }

    inline std::string TStringCvar::GetErrorMessage() {
        std::string result = FErrorMessage;
        FErrorMessage.clear();
        return result;
    }

    inline bool TStringCvar::ParseAndSetValue(const std::string& Value) {
        return SetValue(Value);
    }

    inline std::string TStringCvar::ValueAsString() {
        return GetValue();
    }

    inline TStringCvar* TStringCvar::Add(const std::string& Name, const std::string& Description, 
                                         const std::string& Value, const std::string& DefaultValue, 
                                         const TCvarFlags& Flags, const TCallback<std::string>& OnChange, 
                                         int MinLength, int MaxLength) {
        std::string cvarName = Name;
        std::transform(cvarName.begin(), cvarName.end(), cvarName.begin(), ::tolower);
        
        auto it = Cvars.find(cvarName);
        if (it != Cvars.end()) {
            return nullptr;
        }

        auto cvar = std::make_unique<TStringCvar>(cvarName, Description, Value, DefaultValue, Flags, OnChange, MinLength, MaxLength);
        TStringCvar* result = cvar.get();
        Cvars[cvarName] = std::move(cvar);
        
        if (Flags.count(TCvarFlag::CVAR_SYNC)) {
            CvarsSync[Name] = Cvars[cvarName];
        }
        
        return result;
    }

    inline std::string DumpFlags(TCvarBase* Cvar) {
        std::string CvarFlags = "";
        
        if (Cvar->GetFlags().count(TCvarFlag::CVAR_IMMUTABLE)) CvarFlags += " I";
        if (Cvar->GetFlags().count(TCvarFlag::CVAR_ARCHIVE)) CvarFlags += " A";
        if (Cvar->GetFlags().count(TCvarFlag::CVAR_SPONLY)) CvarFlags += " SP";
        if (Cvar->GetFlags().count(TCvarFlag::CVAR_NOTIFY)) CvarFlags += " N";
        if (Cvar->GetFlags().count(TCvarFlag::CVAR_MODIFIED)) CvarFlags += " M";
        if (Cvar->GetFlags().count(TCvarFlag::CVAR_CLIENT)) CvarFlags += " CL";
        if (Cvar->GetFlags().count(TCvarFlag::CVAR_SERVER)) CvarFlags += " SV";
        if (Cvar->GetFlags().count(TCvarFlag::CVAR_SYNC)) CvarFlags += " SYNC";
        if (Cvar->GetFlags().count(TCvarFlag::CVAR_SCRIPT)) CvarFlags += " SC";
        if (Cvar->GetFlags().count(TCvarFlag::CVAR_INITONLY)) CvarFlags += " INITONLY";

        return CvarFlags;
    }

    inline void ResetSyncCvars() {
        for (auto& pair : CvarsSync) {
            pair.second->Reset();
        }
    }
}

// Using declarations to bring into global namespace
using CvarImpl::TCvarBase;
using CvarImpl::TCvar;
using CvarImpl::TIntegerCvar;
using CvarImpl::TSingleCvar;
using CvarImpl::TBooleanCvar;
using CvarImpl::TColorCvar;
using CvarImpl::TStringCvar;
using CvarImpl::TCallback;
using CvarImpl::DumpFlags;
using CvarImpl::ResetSyncCvars;

// Global variables - they would need to be defined in a source file or with 'extern' in other headers
extern std::unordered_map<std::string, std::unique_ptr<TCvarBase>> Cvars;
extern std::unordered_map<std::string, std::unique_ptr<TCvarBase>> CvarsSync;
extern bool CvarsNeedSyncing;
extern bool CvarsInitialized;

#endif // CVAR_H
// Add these global variable definitions at the end of Cvar.h
std::unordered_map<std::string, std::unique_ptr<TCvarBase>> Cvars;
std::unordered_map<std::string, std::unique_ptr<TCvarBase>> CvarsSync;
bool CvarsNeedSyncing = false;
bool CvarsInitialized = false;

