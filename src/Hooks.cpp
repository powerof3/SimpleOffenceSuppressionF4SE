#include "Hooks.h"

namespace SimpleOffenceSuppression
{
	RE::FIGHT_REACTION GetFactionFightReaction::thunk(RE::Actor* a_subject, RE::Actor* a_player)
	{
		const auto fightReaction = func(a_subject, a_player);
		if (fightReaction == RE::FIGHT_REACTION::kNeutral) {
			if (a_subject && !a_subject->GetHostileToActor(a_player)) {
				const auto settings = Settings::GetSingleton();
				if (settings->onlyCombat && !a_player->IsInCombat()) {
					return fightReaction;
				}
			    if (settings->ignoreCreatures && (a_subject->HasKeyword(actorTypeAnimalKYWD) || a_subject->HasKeyword(actorTypeCreatureKYWD))) {
					return fightReaction;
				}
				if (settings->ignoreFriendlyFire && (a_subject->formFlags & 0x100000) == 0) {
					a_subject->formFlags |= 0x100000;
				}
				return settings->newReaction;
			}
		}
		return fightReaction;
	}

	void InstallOnPostLoad()
	{
		const auto settings = Settings::GetSingleton();
	    settings->Load();

        const REL::Relocation<std::uintptr_t> target{ REL::ID(2231180), 0x1DE };
		stl::write_thunk_call<GetFactionFightReaction>(target.address());

		logger::info("Hooked GetFactionFightReaction");
	}
}
