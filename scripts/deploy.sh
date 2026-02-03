#!/bin/bash
#
# Deploy script for Zephyr Guide Tutorial
#
# This script commits changes and pushes to both repositories:
# - origin: Full project (zephyr-guide-tutorial-openspec)
# - deploy: Docs only for GitHub Pages (zephyr-guide-tutorial)
#
# Usage:
#   ./scripts/deploy.sh "commit message"
#   ./scripts/deploy.sh                    # Uses default message
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get commit message
if [ -n "$1" ]; then
    COMMIT_MSG="$1"
else
    COMMIT_MSG="Update documentation and examples"
fi

echo -e "${YELLOW}=== Zephyr Guide Tutorial Deployment ===${NC}"
echo ""

# Check for changes
if git diff --quiet && git diff --cached --quiet && [ -z "$(git ls-files --others --exclude-standard)" ]; then
    echo -e "${GREEN}No changes to commit.${NC}"

    # Still offer to push if there are unpushed commits
    if [ -n "$(git log origin/main..HEAD 2>/dev/null)" ]; then
        echo -e "${YELLOW}Found unpushed commits. Pushing...${NC}"
    else
        echo "Nothing to push."
        exit 0
    fi
else
    # Stage all changes
    echo -e "${YELLOW}Staging changes...${NC}"
    git add -A

    # Show what will be committed
    echo ""
    echo -e "${YELLOW}Changes to be committed:${NC}"
    git status --short
    echo ""

    # Commit
    echo -e "${YELLOW}Committing...${NC}"
    git commit -m "$COMMIT_MSG

Co-Authored-By: Claude Opus 4.5 <noreply@anthropic.com>"
    echo -e "${GREEN}✓ Committed${NC}"
fi

# Push to origin (main repo)
echo ""
echo -e "${YELLOW}Pushing to origin (zephyr-guide-tutorial-openspec)...${NC}"
git push origin main
echo -e "${GREEN}✓ Pushed to origin${NC}"

# Push docs to deploy repo using subtree
echo ""
echo -e "${YELLOW}Pushing docs to deploy repo (zephyr-guide-tutorial)...${NC}"
git subtree push --prefix=docs deploy main
echo -e "${GREEN}✓ Pushed to deploy${NC}"

echo ""
echo -e "${GREEN}=== Deployment Complete ===${NC}"
echo ""
echo "Repositories updated:"
echo "  • https://github.com/MichaelTien8901/zephyr-guide-tutorial-openspec"
echo "  • https://github.com/MichaelTien8901/zephyr-guide-tutorial"
echo ""
echo "GitHub Pages site:"
echo "  • https://michaeltien8901.github.io/zephyr-guide-tutorial/"
echo ""
