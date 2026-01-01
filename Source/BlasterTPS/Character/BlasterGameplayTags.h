#pragma once

#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

namespace BlasterGameplayTags
{
	// Declare the gameplay tag extern using UE macros (definition will be in BlasterGameplayTags.cpp)
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN (Damage);
	}
}
