#pragma once

#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

namespace BlasterGameplayTags
{
	// Declare the gameplay tag extern using UE macros (definition will be in BlasterGameplayTags.cpp)
	namespace Abilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN (Dash);
	}
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN (Damage);
	}

	namespace State
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN (Stunned);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN (Dashing);
	}

	namespace Cooldown
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN (Dash);
	}
}
