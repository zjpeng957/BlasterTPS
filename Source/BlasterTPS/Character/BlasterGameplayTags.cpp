// Deprecated: BlasterGameplayTags implementation moved to
// Source/BlasterTPS/Private/BlasterGameplayTags.cpp
// Keep this file empty to avoid duplicate symbol definitions.

#include "BlasterGameplayTags.h"

namespace BlasterGameplayTags
{
	namespace Abilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dash, "Abilities.Dash", "Abilities tag used by dash/sprint gameplay ability.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "Abilities.HitReact", "Abilities tag used by hit react gameplay ability.");
	}
	namespace SetByCaller
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "SetByCaller.Damage", "SetByCaller tag used by damage gameplay effects.");
	}

	namespace State
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stunned, "State.Stunned", "Character is stunned and cannot perform certain actions.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dashing, "State.Dashing", "Character is currently sprinting.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Invincible, "State.Invincible", "Character is invincible and cannot be damage");
	}

	namespace Cooldown
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dash, "Cooldown.Dash", "Cooldown applied after using sprint ability.");
	}
}
