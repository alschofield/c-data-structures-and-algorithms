# Learning Repo Source Ownership

This is a learning repository. The user writes all production C by hand.

- Do not create, delete, or modify production implementation files under `src/`: `.c` files other than `test_*.c`, and every `.h` file.
- This includes blank scaffolds. Do not fill headers, add declarations, add include guards, create stub bodies, or translate README contracts into C unless the user explicitly requests that exact source edit.
- Never rewrite, replace, or delete a completed implementation.
- `test_*.c`, `README.md`, `bench/`, and repository tooling are agent-editable when needed or requested.
- Review production source read-only: report findings, let the user make logic changes, then re-review and verify.
