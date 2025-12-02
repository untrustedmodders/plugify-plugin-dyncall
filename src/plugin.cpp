#include <plg/plugin.hpp>
#include <mutex>
#include <plugin_export.h>
#include <dyncall/dyncall.h>

class DynCallPlugin final : public plg::IPluginEntry {
public:
    std::mutex m_mutex;
    std::unordered_map<DCCallVM*, std::vector<std::unique_ptr<std::string>>> m_storage;
} g_dynCallPlugin;

EXPOSE_PLUGIN(PLUGIN_API, DynCallPlugin, &g_dynCallPlugin)

// TODO: Validate ptr and make sure that allocated vm a

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

enum SigChar : char {
	Void        = 'v',
	Bool        = 'B',
	Char        = 'c',
	UChar       = 'C',
	Short       = 's',
	UShort      = 'S',
	Int         = 'i',
	UInt        = 'I',
	Long        = 'j',
	ULong       = 'J',
	LongLong    = 'l',
	ULongLong   = 'L',
	Float       = 'f',
	Double      = 'd',
	Pointer     = 'p',
	String      = 'Z',
	Aggregate   = 'A',
	EndArg      = ')'
};

extern "C" {
	PLUGIN_API DCCallVM* NewVM(size_t size) {
        std::scoped_lock lock(g_dynCallPlugin.m_mutex);
        auto* vm = dcNewCallVM(size);
        g_dynCallPlugin.m_storage[vm] = std::vector<std::unique_ptr<std::string>>();
        return vm;
    }

	PLUGIN_API void Free(DCCallVM* vm) {
        std::scoped_lock lock(g_dynCallPlugin.m_mutex);
        g_dynCallPlugin.m_storage.erase(vm);
        dcFree(vm);
    }

	PLUGIN_API void Reset(DCCallVM* vm) { dcReset(vm); }

	PLUGIN_API void Mode(DCCallVM* vm, int mode) { dcMode(vm, mode); }


	PLUGIN_API void ArgBool(DCCallVM* vm, bool value) { dcArgBool(vm, value); }

	PLUGIN_API void ArgChar(DCCallVM* vm, char value) { dcArgChar(vm, value); }

	PLUGIN_API void ArgInt8(DCCallVM* vm, int8_t value) { dcArgChar(vm, value); }

	PLUGIN_API void ArgUInt8(DCCallVM* vm, uint8_t value) { dcArgChar(vm, static_cast<int8_t>(value)); }

	PLUGIN_API void ArgInt16(DCCallVM* vm, int16_t value) { dcArgShort(vm, value); }

	PLUGIN_API void ArgUInt16(DCCallVM* vm, uint16_t value) { dcArgShort(vm, static_cast<int16_t>(value)); }

	PLUGIN_API void ArgInt32(DCCallVM* vm, int32_t value) { dcArgInt(vm, value); }

	PLUGIN_API void ArgUInt32(DCCallVM* vm, uint32_t value) { dcArgInt(vm, static_cast<int32_t>(value)); }

	PLUGIN_API void ArgInt64(DCCallVM* vm, int64_t value) { dcArgLongLong(vm, value); }

	PLUGIN_API void ArgUInt64(DCCallVM* vm, uint64_t value) { dcArgLongLong(vm, static_cast<int64_t>(value)); }

	PLUGIN_API void ArgFloat(DCCallVM* vm, float value) { dcArgFloat(vm, value); }

	PLUGIN_API void ArgDouble(DCCallVM* vm, double value) { dcArgDouble(vm, value); }

	PLUGIN_API void ArgPointer(DCCallVM* vm, void* value) { dcArgPointer(vm, value); }

	PLUGIN_API void ArgString(DCCallVM* vm, const plg::string& value) {
        std::scoped_lock lock(g_dynCallPlugin.m_mutex);
        const auto& str = *g_dynCallPlugin.m_storage[vm].emplace_back(std::make_unique<std::string>(value));
        dcArgPointer(vm, (DCpointer) str.c_str());
    }

	PLUGIN_API void ArgAggr(DCCallVM* vm, DCaggr* ag, void* value) { dcArgAggr(vm, ag, value); }


	PLUGIN_API void CallVoid(DCCallVM* vm, void* funcptr) { dcCallVoid(vm, funcptr); }

	PLUGIN_API bool CallBool(DCCallVM* vm, void* funcptr) { return dcCallBool(vm, funcptr); }

	PLUGIN_API char CallChar(DCCallVM* vm, void* funcptr) { return dcCallChar(vm, funcptr); }

	PLUGIN_API int8_t CallInt8(DCCallVM* vm, void* funcptr) { return dcCallChar(vm, funcptr); }

	PLUGIN_API uint8_t CallUInt8(DCCallVM* vm, void* funcptr) { return static_cast<uint8_t>(dcCallChar(vm, funcptr)); }

	PLUGIN_API int16_t CallInt16(DCCallVM* vm, void* funcptr) { return dcCallShort(vm, funcptr); }

	PLUGIN_API uint16_t CallUInt16(DCCallVM* vm, void* funcptr) { return static_cast<uint16_t>(dcCallShort(vm, funcptr)); }

	PLUGIN_API int32_t CallInt32(DCCallVM* vm, void* funcptr) { return dcCallInt(vm, funcptr); }

	PLUGIN_API uint32_t CallUInt32(DCCallVM* vm, void* funcptr) { return static_cast<uint32_t>(dcCallInt(vm, funcptr)); }

	PLUGIN_API int64_t CallInt64(DCCallVM* vm, void* funcptr) { return dcCallLongLong(vm, funcptr); }

	PLUGIN_API uint64_t CallUInt64(DCCallVM* vm, void* funcptr) { return static_cast<uint64_t>(dcCallLongLong(vm, funcptr)); }

	PLUGIN_API float CallFloat(DCCallVM* vm, void* funcptr) { return dcCallFloat(vm, funcptr); }

	PLUGIN_API double CallDouble(DCCallVM* vm, void* funcptr) { return dcCallDouble(vm, funcptr); }

	PLUGIN_API void* CallPointer(DCCallVM* vm, void* funcptr) { return dcCallPointer(vm, funcptr); }

	PLUGIN_API plg::string CallString(DCCallVM* vm, void* funcptr) {
        const char* str = static_cast<const char *>(dcCallPointer(vm, funcptr));
        if (str == nullptr)
            return {};
        else
            return str;
    }

	PLUGIN_API void CallAggr(DCCallVM* vm, void* funcptr, DCaggr* ag, void* returnValue) { dcCallAggr(vm, funcptr, ag, returnValue); }

	PLUGIN_API void BeginCallAggr(DCCallVM* vm, DCaggr* ag) { dcBeginCallAggr(vm, ag); }


	PLUGIN_API int GetError(DCCallVM* vm) { return dcGetError(vm); }


	PLUGIN_API DCaggr* NewAggr(size_t fieldCount, size_t size) { return dcNewAggr(fieldCount, size); }

	PLUGIN_API void AggrField(DCaggr* ag, SigChar type, int offset, size_t arrayLength) { dcAggrField(ag, type, offset, arrayLength); }

	PLUGIN_API void CloseAggr(DCaggr* ag) { dcCloseAggr(ag); }

	PLUGIN_API void FreeAggr(DCaggr* ag) { dcFreeAggr(ag); }


	PLUGIN_API int GetModeFromCCSigChar(char sigChar) { return dcGetModeFromCCSigChar(sigChar); }
}

PLUGIFY_WARN_POP()