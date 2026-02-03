# Project Instructions for Claude

## Task Management

**IMPORTANT**: After completing each task, immediately update `openspec/changes/zephyr-guide-tutorial/tasks.md` to mark the task as completed by changing `- [ ]` to `- [x]`. This ensures accurate progress tracking and avoids confusion about what work remains.

## Internal Links Pattern

This Jekyll site uses `{% link %}` tags for all internal links. This ensures:
- Links are validated at build time (Jekyll errors if file doesn't exist)
- URLs automatically use the page's permalink if one is set
- Links work correctly on the live GitHub Pages site

### Syntax

```markdown
[Link Text]({% link path/to/file.md %})
```

### Path Rules

1. **Paths are relative to Jekyll root** (the `docs/` folder)
   - Correct: `{% link part1/01-introduction.md %}`
   - Wrong: `{% link docs/part1/01-introduction.md %}`

2. **Use the actual file path**, not the permalink
   - Correct: `{% link part4/23-advanced-topic/index.md %}`
   - Wrong: `{% link part4/advanced-topic/index.md %}`

3. **Jekyll resolves to permalink** if one is set
   - File: `part4/23-advanced-topic/index.md` with `permalink: /part4/advanced-topic/`
   - Generated URL: `/part4/advanced-topic/`

### Examples

```markdown
# Same directory
[Next Chapter]({% link part1/02-environment-setup.md %})

# Different directory
[Part 3]({% link part3/index.md %})

# Subdirectory with custom permalink
[Advanced Topic]({% link part4/23-advanced-topic/index.md %})
# Resolves to /part4/advanced-topic/ (the permalink)
```

### External Links

For GitHub example code links, use full URLs:
```markdown
[Source Code](https://github.com/MichaelTien8901/zephyr-guide-tutorial/tree/main/examples/ExampleName)
```

## File Structure

```
docs/                    # Jekyll root
├── _config.yml
├── index.md
├── part1/
│   ├── index.md        # permalink: /part1/
│   ├── 01-introduction.md
│   └── ...
├── part4/
│   ├── index.md        # permalink: /part4/
│   └── 23-advanced-topic/    # Folder name has prefix
│       └── index.md    # permalink: /part4/advanced-topic/ (clean URL)
└── examples/
    └── ...             # Zephyr example code
```

## Common Mistakes to Avoid

1. **Don't use `.md` URLs directly** - They won't work on the live site
2. **Don't use `docs/` prefix** - The docs folder IS the Jekyll root
3. **Don't use relative paths like `../`** - Use full path from Jekyll root
4. **Don't guess permalinks** - Use the file path; Jekyll resolves to permalink

## Repository Setup

This project uses two Git remotes:

| Remote | Repository | Purpose |
|:-------|:-----------|:--------|
| `origin` | `zephyr-guide-tutorial-openspec` | Development repo (full project with openspec/) |
| `deploy` | `zephyr-guide-tutorial` | Public GitHub Pages site (docs/ only) |

### Adding the Deploy Remote

If the deploy remote is missing, add it:

```bash
git remote add deploy https://github.com/MichaelTien8901/zephyr-guide-tutorial.git
```

Verify remotes:

```bash
git remote -v
# Should show both origin and deploy
```

## Commit and Push Workflow

After making changes, always push to **both** repositories:

```bash
# 1. Stage and commit changes
git add <files>
git commit -m "Your commit message

Co-Authored-By: Claude Opus 4.5 <noreply@anthropic.com>"

# 2. Push to origin (development repo)
git push origin master

# 3. Push docs/ subtree to deploy repo (public site)
git push deploy $(git subtree split --prefix=docs):main --force
```

### Why Subtree Push?

The development repo structure:
```
zephyr-guide-tutorial-openspec/
├── docs/           # Jekyll site content
├── openspec/       # OpenSpec workflow files
├── CLAUDE.md       # This file (not deployed)
└── README.md       # Project readme (not deployed)
```

The deploy repo expects Jekyll content at root:
```
zephyr-guide-tutorial/
├── _config.yml
├── index.md
├── part1/
├── appendices/
└── .github/workflows/jekyll.yml
```

The subtree push extracts `docs/*` and pushes it as the root of the deploy repo, triggering GitHub Actions to rebuild the Jekyll site.

### Quick Reference

```bash
# Check remotes
git remote -v

# Push to both repos (after commit)
git push origin master
git push deploy $(git subtree split --prefix=docs):main --force

# Check deployment status
gh api repos/MichaelTien8901/zephyr-guide-tutorial/actions/runs --jq '.workflow_runs[0] | "\(.status) \(.conclusion // "running")"'
```
