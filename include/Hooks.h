#pragma once

#include "Settings.h"

namespace SimpleOffenceSuppression
{
	inline RE::BGSKeyword* actorTypeAnimalKYWD{ nullptr };
	inline RE::BGSKeyword* actorTypeCreatureKYWD{ nullptr };

	struct GetFactionFightReaction
	{
		static RE::FIGHT_REACTION                      thunk(RE::Actor* a_subject, RE::Actor* a_player);
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void InstallOnPostLoad();
}
