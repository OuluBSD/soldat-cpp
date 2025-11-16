#ifndef CVAR_H
#define CVAR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <set>
#include <cctype>
#include <algorithm>
#include <sstream>
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

namespace CvarImpl {
    // Forward declarations
    struct TCvarBase;
    template<typename T> struct TCvar;

    // Global variables - these will be defined elsewhere, declared here for use in methods
    extern std::unordered_map<std::string, std::unique_ptr<TCvarBase>> Cvars;
    extern std::unordered_map<std::string, std::unique_ptr<TCvarBase>> CvarsSync;
    extern bool CvarsNeedSyncing;
    extern bool CvarsInitialized;

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

        void SyncUpdate(bool ToSync) {
            if (ToSync) {
                FFlags.insert(TCvarFlag::CVAR_TOSYNC);
                CvarsNeedSyncing = true;
            } else {
                FFlags.erase(TCvarFlag::CVAR_TOSYNC);
            }
        }

        static TCvarBase* Find(const std::string& Name) {
            auto it = Cvars.find(Name);
            if (it != Cvars.end()) {
                return it->second.get();
            }
            return nullptr;
        }

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
        std::function<bool(TCvar<T>*, T)> FOnChange;

    public:
        TCvar(const std::string& Name, const std::string& Description, T Value, T DefaultValue,
              const TCvarFlags& Flags, const std::function<bool(TCvar<T>*, T)>& OnChange)
            : FValue(Value), FDefaultValue(DefaultValue), FOnChange(OnChange) {
            SetName(Name);
            SetDescription(Description);
            SetFlags(Flags);
        }

        // Constructor for default initialization (used by global variables)
        TCvar() : FValue{}, FDefaultValue{}, FOnChange(nullptr) {
            SetName("");
            SetDescription("");
            SetFlags({});
        }

        void Reset() override {
            SetValue(FDefaultValue);
        }

        virtual bool SetValue(T Value) {
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
        void SetOnChange(const std::function<bool(TCvar<T>*, T)>& callback) { FOnChange = callback; }

        // Public getter for accessing the value
        T Value() const { return FValue; }

        // Public access for the callback
        std::function<bool(TCvar<T>*, T)> GetCallback() const { return FOnChange; }
    };

    // Specific Cvar type implementations
    struct TIntCvar : public TCvar<int> {
        TIntCvar() : TCvar<int>() {} // Default constructor for global variables

        TIntCvar(const std::string& Name, const std::string& Description, int Value, int DefaultValue,
                 const TCvarFlags& Flags, const std::function<bool(TCvar<int>*, int)>& OnChange)
            : TCvar<int>(Name, Description, Value, DefaultValue, Flags, OnChange) {}

        bool ParseAndSetValue(const std::string& Value) override {
            try {
                int val = std::stoi(Value);
                return SetValue(val);
            } catch (...) {
                return false;
            }
        }

        std::string ValueAsString() override {
            return std::to_string(GetValue());
        }

        std::string GetErrorMessage() override {
            std::string result = this->FErrorMessage;
            this->FErrorMessage.clear();
            return result;
        }
    };

    struct TFloatCvar : public TCvar<float> {
        TFloatCvar() : TCvar<float>() {} // Default constructor for global variables

        TFloatCvar(const std::string& Name, const std::string& Description, float Value, float DefaultValue,
                   const TCvarFlags& Flags, const std::function<bool(TCvar<float>*, float)>& OnChange)
            : TCvar<float>(Name, Description, Value, DefaultValue, Flags, OnChange) {}

        bool ParseAndSetValue(const std::string& Value) override {
            try {
                float val = std::stof(Value);
                return SetValue(val);
            } catch (...) {
                return false;
            }
        }

        std::string ValueAsString() override {
            return std::to_string(GetValue());
        }

        std::string GetErrorMessage() override {
            std::string result = this->FErrorMessage;
            this->FErrorMessage.clear();
            return result;
        }
    };

    struct TBooleanCvar : public TCvar<bool> {
        TBooleanCvar() : TCvar<bool>() {} // Default constructor for global variables

        TBooleanCvar(const std::string& Name, const std::string& Description, bool Value, bool DefaultValue,
                     const TCvarFlags& Flags, const std::function<bool(TCvar<bool>*, bool)>& OnChange)
            : TCvar<bool>(Name, Description, Value, DefaultValue, Flags, OnChange) {}

        bool ParseAndSetValue(const std::string& Value) override {
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

        std::string ValueAsString() override {
            return GetValue() ? "1" : "0";
        }

        std::string GetErrorMessage() override {
            std::string result = this->FErrorMessage;
            this->FErrorMessage.clear();
            return result;
        }
    };

    struct TColorCvar : public TCvar<TColor> {
        TColorCvar() : TCvar<TColor>() {} // Default constructor for global variables

        TColorCvar(const std::string& Name, const std::string& Description, TColor Value, TColor DefaultValue,
                   const TCvarFlags& Flags, const std::function<bool(TCvar<TColor>*, TColor)>& OnChange)
            : TCvar<TColor>(Name, Description, Value, DefaultValue, Flags, OnChange) {}

        bool ParseAndSetValue(const std::string& Value) override {
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

        std::string ValueAsString() override {
            std::stringstream ss;
            ss << std::hex << GetValue();
            return ss.str();
        }

        std::string GetErrorMessage() override {
            std::string result = this->FErrorMessage;
            this->FErrorMessage.clear();
            return result;
        }
    };

    struct TStringCvar : public TCvar<std::string> {
        TStringCvar() : TCvar<std::string>() {} // Default constructor for global variables

        TStringCvar(const std::string& Name, const std::string& Description, const std::string& Value,
                    const std::string& DefaultValue, const TCvarFlags& Flags, const std::function<bool(TCvar<std::string>*, std::string)>& OnChange)
            : TCvar<std::string>(Name, Description, Value, DefaultValue, Flags, OnChange) {}

        bool ParseAndSetValue(const std::string& Value) override {
            return SetValue(Value);
        }

        std::string ValueAsString() override {
            return GetValue();
        }

        std::string GetErrorMessage() override {
            std::string result = this->FErrorMessage;
            this->FErrorMessage.clear();
            return result;
        }
    };

    void CvarInit();
    std::string DumpFlags(TCvarBase* Cvar);
    void ResetSyncCvars();
}

// Using declarations to bring into global namespace
using CvarImpl::TCvarBase;
using CvarImpl::TCvar;
using CvarImpl::TIntCvar;
using CvarImpl::TFloatCvar;
using CvarImpl::TBooleanCvar;
using CvarImpl::TColorCvar;
using CvarImpl::TStringCvar;
using CvarImpl::CvarInit;
using CvarImpl::DumpFlags;
using CvarImpl::ResetSyncCvars;
using CvarImpl::Cvars;
using CvarImpl::CvarsSync;
using CvarImpl::CvarsNeedSyncing;
using CvarImpl::CvarsInitialized;

#endif // CVAR_H