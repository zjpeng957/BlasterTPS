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
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(GravityTether);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MeleeSingle);
	}
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mana);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TetheredCount);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Duration);
	}

	namespace State
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stunned);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dashing);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Invincible);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Burning);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Targeting);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tethered);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tethering);

	}

	namespace Cooldown
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dash);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Spike);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(GravityTether);
	}

	namespace Input
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TetherSecPress);
	}

	namespace Event
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MeleeHit);
	}
}
