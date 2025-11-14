#include "MapFile.h"
#include <cstring>
#include <algorithm>
#include <physfs.h>
#include <vector>

namespace MapFileImpl {
    // CRC table for checksum computation
    const uint32_t CRCTable[256] = {
        0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
        0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
        0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
        0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
        0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
        0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
        0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
        0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
        0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
        0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
        0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
        0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
        0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
        0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
        0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
        0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
        0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
        0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
        0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
        0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
        0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
        0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
        0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
        0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
        0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
        0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
        0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
        0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
        0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
        0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
        0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
        0xAFB010B1, 0xAB710D06, 0xA6322BD9, 0xA2F3366E, 0xBCB4666B, 0xB8757BDC, 0xB5365D05, 0xB1F740B2
    };

    // Buffer structure to simulate Pascal file buffer
    struct TFileBuffer {
        std::vector<uint8_t> Data;
        size_t Pos;
        
        TFileBuffer() : Pos(0) {}
    };

    inline uint32_t crc32(uint32_t crc, uint8_t* Data, int Len) {
        while (Len > 0) {
            crc = CRCTable[Data[0] ^ ((crc >> 24) & 0xFF)] ^ (crc << 8);
            Data++;
            Len--;
        }
        return crc;
    }

    inline bool ReadAllBytes(const TMapInfo& Map, TFileBuffer& Buffer) {
        Buffer.Data.clear();
        Buffer.Pos = 0;

        if (Map.Name.empty()) {
            return false;
        }

        // Construct file path
        std::string filepath = "maps/" + Map.Name + ".pms";
        PHYSFS_File* file = PHYSFS_openRead(filepath.c_str());

        if (!file) {
            // Try mounting new map if not found in default location
            PHYSFS_removeFromSearchPath("/current_map");
            if (!PHYSFS_mount(Map.Path.c_str(), "/current_map", 1)) {
                return false;
            }
            filepath = "/current_map/maps/" + Map.MapName + ".pms";
            file = PHYSFS_openRead(filepath.c_str());
            if (!file) {
                return false;
            }
        }

        // Get file size
        PHYSFS_sint64 fileSize = PHYSFS_fileLength(file);
        if (fileSize < 0) {
            PHYSFS_close(file);
            return false;
        }

        // Read entire file into buffer
        Buffer.Data.resize(fileSize);
        PHYSFS_readBytes(file, Buffer.Data.data(), fileSize);
        PHYSFS_close(file);

        Buffer.Pos = 0;  // Reset position to beginning
        return true;
    }

    inline void BufferRead(TFileBuffer& bf, uint8_t* Dest, size_t Size) {
        if (bf.Pos + Size > bf.Data.size()) {
            // If we're past the end, fill with zeros
            std::fill(Dest, Dest + Size, 0);
        } else {
            // Copy from buffer
            std::memcpy(Dest, bf.Data.data() + bf.Pos, Size);
        }
        bf.Pos += Size;
    }

    inline uint8_t ReadUint8(TFileBuffer& bf) {
        uint8_t result = 0;
        BufferRead(bf, reinterpret_cast<uint8_t*>(&result), sizeof(result));
        return result;
    }

    inline uint16_t ReadUint16(TFileBuffer& bf) {
        uint16_t result = 0;
        BufferRead(bf, reinterpret_cast<uint8_t*>(&result), sizeof(result));
        return result;
    }

    inline int32_t ReadInt32(TFileBuffer& bf) {
        int32_t result = 0;
        BufferRead(bf, reinterpret_cast<uint8_t*>(&result), sizeof(result));
        return result;
    }

    inline float ReadSingle(TFileBuffer& bf) {
        float result = 0.0f;
        BufferRead(bf, reinterpret_cast<uint8_t*>(&result), sizeof(result));
        return result;
    }

    inline std::string ReadString(TFileBuffer& bf, int MaxSize) {
        std::string result = "";
        uint8_t n = ReadUint8(bf);

        if (n < 128 && n <= MaxSize && (bf.Pos + n) <= bf.Data.size()) {
            result.assign(reinterpret_cast<char*>(bf.Data.data() + bf.Pos), n);
            bf.Pos += n;
        } else {
            // Skip ahead if string is too long
            bf.Pos += MaxSize;
        }

        return result;
    }

    inline TVector3 ReadVec3(TFileBuffer& bf) {
        TVector3 result;
        result.x = ReadSingle(bf);
        result.y = ReadSingle(bf);
        result.z = ReadSingle(bf);
        return result;
    }

    inline TMapColor ReadColor(TFileBuffer& bf) {
        TMapColor result;
        result[2] = ReadUint8(bf);  // Red
        result[1] = ReadUint8(bf);  // Green
        result[0] = ReadUint8(bf);  // Blue
        result[3] = ReadUint8(bf);  // Alpha
        return result;
    }

    inline TMapVertex ReadVertex(TFileBuffer& bf) {
        TMapVertex result;
        result.x = ReadSingle(bf);
        result.y = ReadSingle(bf);
        result.z = ReadSingle(bf);
        result.rhw = ReadSingle(bf);
        
        // Read color
        result.Color[2] = ReadUint8(bf);  // Red
        result.Color[1] = ReadUint8(bf);  // Green
        result.Color[0] = ReadUint8(bf);  // Blue
        result.Color[3] = ReadUint8(bf);  // Alpha
        
        result.u = ReadSingle(bf);
        result.v = ReadSingle(bf);
        
        return result;
    }

    inline TMapColor MapColor(int32_t Color) {
        TMapColor result;
        result[0] = (Color >> (0 * 8)) & 0xFF;  // Blue
        result[1] = (Color >> (1 * 8)) & 0xFF;  // Green
        result[2] = (Color >> (2 * 8)) & 0xFF;  // Red
        result[3] = (Color >> (3 * 8)) & 0xFF;  // Alpha
        return result;
    }

    inline bool LoadMapFile(const TMapInfo& MapInfo, TMapFile& Map) {
        TFileBuffer bf;
        bf.Data.clear();
        bf.Pos = 0;

        if (!ReadAllBytes(MapInfo, bf)) {
            return false;
        }

        // Read header/options
        Map.MapInfo = MapInfo;
        Map.Filename = MapInfo.Name;
        Map.Version = ReadInt32(bf);
        Map.MapName = ReadString(bf, 38);
        
        // Read first texture name
        std::vector<std::string> textures;
        textures.push_back(ReadString(bf, 24));
        Map.Textures = textures;
        
        Map.BgColorTop = ReadColor(bf);
        Map.BgColorBtm = ReadColor(bf);
        Map.StartJet = ReadInt32(bf);
        Map.GrenadePacks = ReadUint8(bf);
        Map.Medikits = ReadUint8(bf);
        Map.Weather = ReadUint8(bf);
        Map.Steps = ReadUint8(bf);
        Map.RandomID = ReadInt32(bf);

        // Read polygons
        int n = ReadInt32(bf);

        if ((n > MAX_POLYS) || (n < 0)) {
            return false;
        }

        Map.Polygons.resize(n);
        for (int i = 0; i < n; i++) {
            Map.Polygons[i].Vertices[1] = ReadVertex(bf);
            Map.Polygons[i].Vertices[2] = ReadVertex(bf);
            Map.Polygons[i].Vertices[3] = ReadVertex(bf);
            Map.Polygons[i].Normals[1] = ReadVec3(bf);
            Map.Polygons[i].Normals[2] = ReadVec3(bf);
            Map.Polygons[i].Normals[3] = ReadVec3(bf);
            Map.Polygons[i].PolyType = ReadUint8(bf);
            Map.Polygons[i].TextureIndex = 0; // Default value
        }

        // Read sectors
        Map.SectorsDivision = ReadInt32(bf);
        Map.SectorsNum = ReadInt32(bf);

        if ((Map.SectorsNum > MAX_SECTOR) || (Map.SectorsNum < 0)) {
            return false;
        }

        int sect_count = (2 * Map.SectorsNum + 1) * (2 * Map.SectorsNum + 1);
        Map.Sectors.resize(sect_count);

        for (int i = 0; i < sect_count; i++) {
            int m = ReadUint16(bf);

            if (m > MAX_POLYS) {
                return false;
            }

            Map.Sectors[i].Polys.resize(m);
            for (int j = 0; j < m; j++) {
                Map.Sectors[i].Polys[j] = ReadUint16(bf);
            }
        }

        // Read props
        n = ReadInt32(bf);

        if ((n > MAX_PROPS) || (n < 0)) {
            return false;
        }

        Map.Props.resize(n);
        for (int i = 0; i < n; i++) {
            Map.Props[i].Active = (ReadUint8(bf) != 0);
            bf.Pos += 1; // Skip padding
            Map.Props[i].Style = ReadUint16(bf);
            Map.Props[i].Width = ReadInt32(bf);
            Map.Props[i].Height = ReadInt32(bf);
            Map.Props[i].x = ReadSingle(bf);
            Map.Props[i].y = ReadSingle(bf);
            Map.Props[i].Rotation = ReadSingle(bf);
            Map.Props[i].ScaleX = ReadSingle(bf);
            Map.Props[i].ScaleY = ReadSingle(bf);
            Map.Props[i].Alpha = ReadUint8(bf);
            bf.Pos += 3; // Skip padding
            Map.Props[i].Color = ReadColor(bf);
            Map.Props[i].Level = ReadUint8(bf);
            bf.Pos += 3; // Skip padding
        }

        // Read scenery
        n = ReadInt32(bf);

        if ((n > MAX_PROPS) || (n < 0)) {
            return false;
        }

        Map.Scenery.resize(n);
        for (int i = 0; i < n; i++) {
            Map.Scenery[i].Filename = ReadString(bf, 50);
            Map.Scenery[i].Date = ReadInt32(bf);
        }

        // Read colliders
        n = ReadInt32(bf);

        if ((n > MAX_COLLIDERS) || (n < 0)) {
            return false;
        }

        Map.Colliders.resize(n);
        for (int i = 0; i < n; i++) {
            Map.Colliders[i].Active = (ReadUint8(bf) != 0);
            bf.Pos += 3; // Skip padding
            Map.Colliders[i].x = ReadSingle(bf);
            Map.Colliders[i].y = ReadSingle(bf);
            Map.Colliders[i].Radius = ReadSingle(bf);
        }

        // Read spawnpoints
        n = ReadInt32(bf);

        if ((n > MAX_SPAWNPOINTS) || (n < 0)) {
            return false;
        }

        Map.Spawnpoints.resize(n);
        for (int i = 0; i < n; i++) {
            Map.Spawnpoints[i].Active = (ReadUint8(bf) != 0);
            bf.Pos += 3; // Skip padding
            Map.Spawnpoints[i].x = ReadInt32(bf);
            Map.Spawnpoints[i].y = ReadInt32(bf);
            Map.Spawnpoints[i].Team = ReadInt32(bf);
        }

        // Read waypoints
        n = ReadInt32(bf);

        if ((n > MAX_WAYPOINTS) || (n < 0)) {
            return false;
        }

        Map.Waypoints.resize(n);
        for (int i = 0; i < n; i++) {
            Map.Waypoints[i].Active = (ReadUint8(bf) != 0);
            bf.Pos += 3; // Skip padding
            Map.Waypoints[i].id = ReadInt32(bf);
            Map.Waypoints[i].x = ReadInt32(bf);
            Map.Waypoints[i].y = ReadInt32(bf);
            Map.Waypoints[i].Left = (ReadUint8(bf) != 0);
            Map.Waypoints[i].Right = (ReadUint8(bf) != 0);
            Map.Waypoints[i].Up = (ReadUint8(bf) != 0);
            Map.Waypoints[i].Down = (ReadUint8(bf) != 0);
            Map.Waypoints[i].M2 = (ReadUint8(bf) != 0);
            Map.Waypoints[i].PathNum = ReadUint8(bf);
            Map.Waypoints[i].C1 = ReadUint8(bf);
            Map.Waypoints[i].C2 = ReadUint8(bf);
            Map.Waypoints[i].C3 = ReadUint8(bf);
            bf.Pos += 3; // Skip padding
            Map.Waypoints[i].ConnectionsNum = ReadInt32(bf);

            for (int j = 1; j <= MAX_CONNECTIONS; j++) {
                Map.Waypoints[i].Connections[j] = ReadInt32(bf);
            }
        }

        // Calculate hash checksum
        Map.Hash = crc32(5381, bf.Data.data(), static_cast<int>(bf.Data.size()));

        return true;
    }

    inline bool IsPropActive(TMapFile& Map, int Index) {
        if (Index < 0 || Index >= static_cast<int>(Map.Props.size())) {
            return false;
        }

        TMapProp& prop = Map.Props[Index];
        return prop.Active && (prop.Level <= 2) && (prop.Style > 0) && 
               (prop.Style < static_cast<int>(Map.Scenery.size()));
    }
}

// Using declarations to bring functions into global namespace
using MapFileImpl::TMapColor;
using MapFileImpl::TMapVertex;
using MapFileImpl::TMapPolygon;
using MapFileImpl::TMapSector;
using MapFileImpl::TMapProp;
using MapFileImpl::TMapScenery;
using MapFileImpl::TMapCollider;
using MapFileImpl::TMapSpawnpoint;
using MapFileImpl::TMapFile;
using MapFileImpl::LoadMapFile;
using MapFileImpl::MapColor;
using MapFileImpl::IsPropActive;