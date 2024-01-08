#include "Structs/OSVerInfo.h"

FOSVerInfo::FOSVerInfo(HarmonyLinkLib::FOSVerInfo* oldInfo)
{
	if (oldInfo)
	{
		Name = oldInfo->name.c_str();
		Version = oldInfo->version;
		ID = oldInfo->id.c_str();
		PrettyName = oldInfo->pretty_name.c_str();
		VariantID = oldInfo->variant_id.c_str();
		VersionID = oldInfo->version_id.c_str();
		
		oldInfo->free();
	}
}
