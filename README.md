Soar-SC
=======

Soar-SC stands for Soar-StarCraft.  Soar-SC is an Artificial Intelligence (AI) research platform made,
maintained, and updated by Alex Turner.  The AI plays StarCraft: Brood War (Not StarCraft 2!).  The
platform consists of the middleware and an agent capable of playing and beating the built-in StarCraft
AIs on up to and including the 'Difficult' setting (the 2nd hardest difficultly behind 'Insane').

Soar-SC uses the Soar Cognitive Architecture made at the University of Michigan.  It also uses an
extension to Soar called SVS which stands for Spatial Vision System.  SVS allows a Soar Agent to do
spatial reasoning.  Soar-SC uses a beta version of SVS and because of this there are still some bugs
that SVS is the cause of.

Soar-SC also uses the Brood War Application Programing Interface, shorten to BWAPI.  It uses version 4
of BWAPI, currently in beta.  This means that there are bugs that occur which are not related to Soar-SC
but will cause the entire system to crash occasionally.  These bugs will hopefully be fixed by the time
BWAPI v4 is out of beta and is stable.

The middleware of Soar-SC interfaces between BWAPI and Soar, which contains the AI portion of Soar-Sc.
It does this by having two seperate queues and seperate threads which a class, called Soar-SC, keeps in
sync and maintains.  Soar_Link is the link to Soar and BWAPI_Link is the link to BWAPI.

Soar-SC also uses a custom viewer for SVS which is easier to use in a Real-Time Strategy Game (RTS) like
StarCraft.  The custom viewer is based on Zenilib v4, written by a University of Michigan graduate
student.

The AI Portion of Soar-SC can gather minerals, build units such as SCVs and marines, build and place
buildings without the need to have predetermined locations for them and will attack in "groups" of marines.
The AI can only play as Terran currently.

List of current goals and work
==============================

Completed goals and work
------------------------

- Better Terrain Algorithm for parsing a map (flood fill)

- Displaying updates about the Agent through the StarCraft user interface

- By default using watch 0 and outputting using RHS rules

- Virtual Objects (No more predetermined build locations)

- Reoccurring Scouting (The AI will continuously scout a map even after it has explored the entire map)

- No more rushing (The AI will not attack as soon as it build marines)

Future Work
-----------

- Group Unit attacks (no longer attack units with only 1 other unit unless desired)

- Attack closest enemy (better attack strategy)

- Add middleware support for playing as Protoss or Zerg (Almost there right now but there are bugs and other
  minor issues to fix)

- Fix the bugs regarding the AI playing against enemies other than Terran

- Add support for attacking in concaves (concaves are when you attack using a group of units and using the
  group to surround the enemy in a half/full circle as you attack)

- No longer use GetCommand and a few other thigns so we can optimize the middleware and Soar to be even faster
  at running the agent.  This includings no longer tracking output link or input-link changes.

- Make the middleware AI architecture independent (ie. make it easy to switch between arbitrary architectures
  such as CSoar and JSoar).

Very far Future work
--------------------

- Better attack strategies

- Better base building strategies

- Better scouting strategies

- Reinforcement learning

- Utilize the other memories of Soar such as semantic and episodic memory
