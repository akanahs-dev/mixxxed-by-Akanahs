# Scratch Sensei V1 is loaded-deck Prepare Track only

Status: accepted

Scratch Sensei V1 should focus on one job: helping the DJ inspect the currently loaded deck track.

V1 should not support library-row context, Practice Skills, lesson catalogs, track-pair workflows, external export, or automatic deck manipulation.

The future training platform remains a product goal. V1 should reserve space for it through reusable analysis summaries, stable suggestion identifiers, and clean module boundaries rather than by shipping lesson UI early.

## Consequences

- If no deck has a track, Scratch Sensei shows an empty state asking the user to load a deck.
- If one deck is loaded, Scratch Sensei uses that deck.
- If multiple decks are loaded and active deck context is ambiguous, Scratch Sensei shows a small Deck 1 / Deck 2 selector.
- Practice Skills and lesson work move to a later version after Prepare Track is trustworthy.
- The V1 UI has one workspace, not two large tabs.
- Future Practice Skills should consume V1 analysis capabilities instead of depending on Prepare Track UI internals.
