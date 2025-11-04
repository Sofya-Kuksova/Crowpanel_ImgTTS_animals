#include "builtin_texts.h"
#include <stdatomic.h>

static _Atomic(builtin_text_case_t) s_current_case = CASE_TXT_01;

const char* get_builtin_text(void)
{
    switch ((builtin_text_case_t)atomic_load(&s_current_case)) {
        case CASE_TXT_01:
            return "Yes, it's me, the flamingo!\n"
                   "I balance on one leg to keep my body warm and my muscles relaxed.\n"
                   "When I sleep, I sometimes wobble, but the wind holds me steady.\n"
                   "Try it yourself, one leg, eyes closed, and a dream about pink clouds!";
        case CASE_TXT_02: 
            return "I'm the camel, the traveler of endless sand.\n"
                   "My hump stores fat, not water, and turns it into energy when I need it most.\n"
                   "I can walk for days while others hide from the heat.\n"
                   "And yes, I blink at sandstorms like they're polite conversations.";
        case CASE_TXT_03: 
            return "I'm the octopus, the ocean's quick-change artist.\n"
                   "My skin is covered with cells that paint me into coral or shadow.\n"
                   "I can open jars, solve puzzles, and sometimes sneak out for adventures.\n"
                   "If you ever lose me, check the nearest teapot.";
        case CASE_TXT_04: 
            return "That's me, the hedgehog!\n"
                   "I curl up tight so no one dares to touch my soft heart.\n"
                   "Inside my prickles I wait, listening for peace to return.\n"
                   "When it does, I uncurl slowly, like morning waking up.";
        case CASE_TXT_05: 
            return "I'm the chameleon, a walking rainbow with feelings.\n"
                   "When I'm calm, I turn green. When I'm excited, I sparkle like sunrise.\n"
                   "My eyes move in two directions, so I never miss a snack.\n"
                   "Sometimes I change color just to see your surprised face.";
        case CASE_TXT_06: 
            return "I'm the dolphin, the cheerful chatter of the sea.\n"
                   "We speak in clicks and whistles that travel faster than light in water.\n"
                   "Each sound means something. A greeting, a name, or a game.\n"
                   "If you wave to me, I might answer with a splash!";
        case CASE_TXT_07: 
            return "I'm the ant, the strongest worker you'll never notice.\n"
                   "My friends and I build tunnels deeper than you can imagine.\n"
                   "We carry food, stones, and dreams of being giants.\n"
                   "When we march, the world trembles, just a little.";
        case CASE_TXT_08: 
            return "I'm the owl, the silent reader of the night.\n"
                   "My eyes catch even the tiniest sparkle of moonlight.\n"
                   "I can turn my head almost all the way around, no peeking rules apply.\n"
                   "When you sleep, I'm out studying the stars.";
        case CASE_TXT_09: 
            return "I'm the penguin, the tuxedo swimmer of the ice.\n"
                   "My wings became flippers, and I fly through water instead of air.\n"
                   "We huddle together when the wind gets mean.\n"
                   "And when we walk, yes, we know it looks funny. We like it!";
        case CASE_TXT_10: 
            return "I'm the elephant, the gentle giant of memory.\n"
                   "I know the paths to water, the voices of friends, and the smell of rain.\n"
                   "My trunk is my hand. my nose. And sometimes my trumpet.\n"
                   "If you tell me a secret, I'll keep it forever.";

        /* Example for an added custom case:
        case CASE_TXT_11:
            return "Your custom TTS text goes here.\n"
                   "Keep it descriptive but not too long.";
        */
        default:
            return "";
    }
}

void builtin_text_next(void)
{
    builtin_text_case_t cur = atomic_load(&s_current_case);
    builtin_text_case_t nxt = (cur + 1) % CASE_TXT_COUNT;
    atomic_store(&s_current_case, nxt);
}

void builtin_text_set(builtin_text_case_t c)
{
    if (c >= 0 && c < CASE_TXT_COUNT) {
        atomic_store(&s_current_case, c);
    }
}

builtin_text_case_t builtin_text_get(void)
{
    return atomic_load(&s_current_case);
}
