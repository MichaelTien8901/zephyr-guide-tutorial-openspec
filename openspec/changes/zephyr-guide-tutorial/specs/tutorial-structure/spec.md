## ADDED Requirements

### Requirement: Jekyll site with just-the-docs theme
The documentation site SHALL use Jekyll with the just-the-docs theme for consistent navigation and search functionality.

#### Scenario: Site builds successfully
- **WHEN** running `bundle exec jekyll build` in the docs directory
- **THEN** the site compiles without errors and generates static HTML

#### Scenario: Navigation works correctly
- **WHEN** a user visits the site
- **THEN** they see a left sidebar with all parts and chapters organized hierarchically

### Requirement: Mermaid diagram support
The site SHALL render Mermaid diagrams with a paper-like background theme for readability on dark backgrounds.

#### Scenario: Mermaid diagrams render
- **WHEN** a page contains a mermaid code block
- **THEN** the diagram renders with #d5d0c8 background and dark gray text

#### Scenario: Mermaid configuration loaded
- **WHEN** the site loads
- **THEN** mermaid.js initializes with custom theme variables from mermaid_config.js

### Requirement: GitHub Pages hosting configuration
The site SHALL be deployable to GitHub Pages with correct baseurl configuration.

#### Scenario: Site deploys to GitHub Pages
- **WHEN** the docs subtree is pushed to the deploy repository
- **THEN** GitHub Actions builds and publishes the Jekyll site

#### Scenario: Internal links resolve correctly
- **WHEN** a page uses {% link %} tags for internal navigation
- **THEN** links resolve to correct URLs on the live site

### Requirement: Part-based content organization
Content SHALL be organized into 7 parts plus appendices, progressing from beginner to professional.

#### Scenario: Parts are navigable
- **WHEN** a user views the home page
- **THEN** they see links to all 7 parts with descriptions and difficulty levels

#### Scenario: Each part has index page
- **WHEN** a user navigates to a part
- **THEN** they see an index page listing all chapters in that part

### Requirement: Responsive design
The site SHALL be readable on mobile devices and desktop browsers.

#### Scenario: Mobile navigation
- **WHEN** viewing on a mobile device
- **THEN** the navigation collapses to a hamburger menu

### Requirement: Search functionality
The site SHALL provide search across all content.

#### Scenario: Search returns results
- **WHEN** a user searches for "thread"
- **THEN** search results show all pages mentioning threads with context previews
