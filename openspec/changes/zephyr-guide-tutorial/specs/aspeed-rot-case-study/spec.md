## ADDED Requirements

### Requirement: ASPEED RoT project overview
The guide SHALL introduce ASPEED's Root of Trust implementation as a security-focused Zephyr example.

#### Scenario: What is ASPEED RoT
- **WHEN** a reader starts the case study
- **THEN** they understand ASPEED RoT provides hardware-backed security for server platforms

#### Scenario: RoT architecture diagram
- **WHEN** viewing RoT section
- **THEN** a mermaid diagram shows RoT's security architecture and trust boundaries

#### Scenario: Why study ASPEED RoT
- **WHEN** a reader considers the case study value
- **THEN** they understand it demonstrates security-critical Zephyr development patterns

### Requirement: Security architecture analysis
The guide SHALL explain ASPEED RoT's security design.

#### Scenario: Trust chain
- **WHEN** a reader learns trust architecture
- **THEN** they understand immutable bootloader → verified firmware → runtime attestation chain

#### Scenario: Security boundaries
- **WHEN** a reader studies isolation
- **THEN** they understand how RoT isolates secure and non-secure code

#### Scenario: Threat model
- **WHEN** a reader understands threats
- **THEN** they understand what attacks RoT defends against

### Requirement: Secure boot implementation
The guide SHALL explain secure boot patterns.

#### Scenario: Boot verification
- **WHEN** a reader learns secure boot
- **THEN** they understand signature verification, key management, and boot flow

#### Scenario: Secure boot diagram
- **WHEN** viewing secure boot section
- **THEN** a mermaid flowchart shows ROM → OTP keys → verify → execute stages

#### Scenario: Recovery mechanisms
- **WHEN** a reader learns recovery
- **THEN** they understand failover and recovery image handling

### Requirement: Attestation implementation
The guide SHALL explain remote attestation patterns.

#### Scenario: Attestation basics
- **WHEN** a reader learns attestation
- **THEN** they understand measurement, quote generation, and verification concepts

#### Scenario: DICE/SPDM overview
- **WHEN** a reader studies protocols
- **THEN** they understand DICE attestation and SPDM protocol usage

#### Scenario: Attestation flow diagram
- **WHEN** viewing attestation section
- **THEN** a mermaid sequence diagram shows attestation request → measurement → response

### Requirement: Firmware protection (PFR)
The guide SHALL explain Platform Firmware Resilience patterns.

#### Scenario: PFR concepts
- **WHEN** a reader learns PFR
- **THEN** they understand protection, detection, and recovery roles of PFR

#### Scenario: Firmware update security
- **WHEN** a reader studies updates
- **THEN** they understand secure update verification and anti-rollback

#### Scenario: Active/recovery image management
- **WHEN** a reader studies resilience
- **THEN** they understand dual-image patterns and corruption recovery

### Requirement: Cryptographic subsystem usage
The guide SHALL explain crypto usage in RoT.

#### Scenario: Hardware crypto
- **WHEN** a reader learns crypto acceleration
- **THEN** they understand ASPEED crypto engine integration

#### Scenario: Key storage
- **WHEN** a reader studies key management
- **THEN** they understand OTP, secure storage, and key hierarchy

### Requirement: Zephyr security features utilized
The guide SHALL highlight Zephyr security features used by RoT.

#### Scenario: Memory protection
- **WHEN** a reader learns security features
- **THEN** they understand how RoT uses MPU/MMU and userspace

#### Scenario: Secure coding patterns
- **WHEN** a reader studies code patterns
- **THEN** they understand input validation, buffer handling, and timing attack mitigation

### Requirement: Lessons learned from ASPEED RoT
The guide SHALL extract security patterns applicable to other projects.

#### Scenario: Security design principles
- **WHEN** a reader concludes the case study
- **THEN** they have a checklist of security considerations for Zephyr projects

#### Scenario: Certification considerations
- **WHEN** a reader thinks about compliance
- **THEN** they understand how RoT patterns relate to security certifications
