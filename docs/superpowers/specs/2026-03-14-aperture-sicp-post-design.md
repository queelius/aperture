# Aperture Blog Post Rewrite — SICP Series

**Date:** 2026-03-14
**Target file:** `~/github/repos/metafunctor/content/post/2024-04-01-aperture/index.md`
**Action:** Rewrite in place (same date, same URL)

## Goal

Rewrite the existing aperture blog post to frame it through the SICP lens ("build a language to solve a problem"), reference the Go implementation with real CLI examples, and add it to the SICP series.

## Structure

**Act 1: The Language (~60%)**
- SICP hook: when coordination across distributed parties is complex enough, build a language
- Primitives: values, symbols, holes (`?x`, `?ns.x`)
- Combination: S-expressions, application, partial evaluation as "combination that tolerates unknowns"
- Abstraction: lambda/let/define, plus named holes as deferred values
- Closure property: partial-eval yields an expression, fill yields an expression — always the same kind of thing
- Real `aperture` CLI examples throughout

**Act 2: The Problem (~30%)**
- Distributed coordination problem: server has CPU, client has data
- Local computation pattern with real CLI walkthrough
- Brief limitations (one honest paragraph, not a full section)

**Closing (~10%)**
- SICP connection: the language made coordination *thinkable*
- Links to paper, repo, spec

## Front Matter Changes

- Add `series: ["sicp"]`, `series_weight: 6`
- Add tags: `sicp`, `dsl`, `metalinguistic-abstraction`
- Remove tags: `distributed-systems` (too broad)
- Keep: `partial-evaluation`, `coordination`, `programming-languages`
- Keep: `linked_project: ["aperture"]`

## Content Changes

- C++ references → Go with actual `aperture eval/partial/fill` output
- Remove performance table (benchmarks from defunct C++ impl)
- Paper-summary structure → SICP narrative arc
- Pseudo-code → real runnable commands
- Condense limitations to one paragraph (honest but not dominant)

## Verified CLI Examples

```
$ aperture eval "(+ 3 5)"
8

$ aperture partial /tmp/test.apt  # containing (+ 3 ?x 5)
(+ 8 ?x)

$ aperture fill --hole x=10 /tmp/test.apt
18

$ aperture partial /tmp/zero.apt  # containing (* 0 ?anything)
0

$ aperture fill --hole a=5 /tmp/multi.apt  # containing (+ ?a (* 2 ?b) 10)
(+ 15 (* 2 ?b))

$ aperture fill --hole a=5 --hole b=3 /tmp/multi.apt
21
```
