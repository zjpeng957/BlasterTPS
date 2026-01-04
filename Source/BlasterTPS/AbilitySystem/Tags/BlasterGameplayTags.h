#pragma once

#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

namespace BlasterGameplayTags
{
	// Declare the gameplay tag extern using UE macros (definition will be in BlasterGameplayTags.cpp)
	namespace Abilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dash);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Spike);
	}
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage);
	}

	namespace State
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stunned);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dashing);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Burning);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Targeting);
	}

	namespace Cooldown
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dash);
	}
}
