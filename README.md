# Data Structures and Algorithms in C

## Purpose and Status

A language-specific, from-first-principles data-structures-and-algorithms learning curriculum.
Implemented learning repository. A* and Kruskal remain contract-first learner targets.

## Curriculum Coverage

28 topic leaves: the full curriculum, including the dynamic-array and Kruskal exercises.

## Commands

```sh
make test NAME=data-structures/linear/stacks/stack
make benchmark NAME=data-structures/linear/stacks/stack BENCHMARK=stack
make benchmark-test
```

## Conventions

The dynamic-array leaf is an explicit curriculum exercise.
Use opaque types and caller-owned `void *` values where a contract specifies them. Do not substitute library containers, search routines, or sorting routines.

## Documentation Contract Template

Each topic leaf README uses these sections: `Implementation Status`, `How It Works`, `Required API`, `Contract`, `Complexity Targets`, and `Verification`. The leaf README is authoritative for that topic; source and tests must preserve its language-specific API syntax and stated behavior.

## Repository-Specific Notes

Production implementations are learner-owned. Documentation and verification scaffolding may describe the required work but do not substitute for it.
