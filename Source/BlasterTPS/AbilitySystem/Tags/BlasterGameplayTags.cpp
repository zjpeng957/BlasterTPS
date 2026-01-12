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
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Spike, "Abilities.Spike", "Abilities tag used by Spike gameplay ability.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GravityTether, "Abilities.GravityTether", "Abilities tag used by Gravity Tether gameplay ability.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeSingle, "Abilities.MeleeSingle", "Abilities tag used by MeleeSingle gameplay ability.");
	}
	namespace SetByCaller
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "SetByCaller.Damage", "SetByCaller tag used by damage gameplay effects.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mana, "SetByCaller.Mana", "SetByCaller tag used by mana gameplay effects.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TetheredCount, "SetByCaller.TetheredCount", "SetByCaller tag used by tether gameplay effects.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Duration, "SetByCaller.Duration", "SetByCaller tag used to pass duration.");
	}

	namespace State
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stunned, "State.Stunned", "Character is stunned and cannot perform certain actions.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dashing, "State.Dashing", "Character is currently sprinting.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Invincible, "State.Invincible", "Character is invincible and cannot be damage");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Burning, "State.Burning", "Character is Burning");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Targeting, "State.Targeting", "Character is currently targeting an ability.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tethered, "State.Tethered", "Character is currently Tethered by an ability.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tethering, "State.Tethering", "Character is currently using Gravity Tether ability.");
	}

	namespace Cooldown
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dash, "Cooldown.Dash", "Cooldown applied after using dash ability.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Spike, "Cooldown.Spike", "Cooldown applied after using Spike ability.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GravityTether, "Cooldown.GravityTether", "Cooldown applied after using Gravity Tether ability.");
	}
	namespace Input
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TetherSecPress, "Input.TetherSecPress", "Input tag send after press Gravity Tether second time.");
	}

	namespace Event
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeHit, "Event.MeleeHit", "Event tag send after ability melee attack.");
	}
}
