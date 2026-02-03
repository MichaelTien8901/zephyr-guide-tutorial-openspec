## ADDED Requirements

### Requirement: Memory allocation overview
The guide SHALL explain Zephyr's memory allocation options with comparison diagram.

#### Scenario: Allocation strategies compared
- **WHEN** viewing memory management
- **THEN** a mermaid diagram compares heap, pool, slab allocators with trade-offs

#### Scenario: When to use which allocator
- **WHEN** a reader chooses allocation strategy
- **THEN** a decision flowchart guides based on size variability and performance needs

### Requirement: Kernel heap documentation
The guide SHALL explain the kernel heap (k_heap) usage.

#### Scenario: Heap initialization
- **WHEN** a reader uses heap allocation
- **THEN** they understand K_HEAP_DEFINE, k_heap_alloc, k_heap_free

#### Scenario: System heap
- **WHEN** a reader uses malloc/free
- **THEN** they understand CONFIG_HEAP_MEM_POOL_SIZE and k_malloc/k_free

#### Scenario: Heap fragmentation
- **WHEN** a reader learns heap limitations
- **THEN** they understand fragmentation risks and mitigation strategies

### Requirement: Memory pools documentation
The guide SHALL explain fixed-size block pools.

#### Scenario: Memory pool basics
- **WHEN** a reader uses memory pools
- **THEN** they understand k_mem_pool and fixed-block allocation benefits

#### Scenario: Pool sizing
- **WHEN** a reader configures pools
- **THEN** they understand block size, block count, and alignment requirements

### Requirement: Memory slabs documentation
The guide SHALL explain memory slabs for uniform allocations.

#### Scenario: Slab allocator
- **WHEN** a reader uses slabs
- **THEN** they understand K_MEM_SLAB_DEFINE, k_mem_slab_alloc, k_mem_slab_free

#### Scenario: Slab vs pool decision
- **WHEN** a reader chooses between slab and pool
- **THEN** they understand slabs are simpler for single-size blocks

### Requirement: Stack memory management
The guide SHALL explain thread stack allocation.

#### Scenario: Stack sizing
- **WHEN** a reader sizes thread stacks
- **THEN** they understand CONFIG_MAIN_STACK_SIZE, K_THREAD_STACK_DEFINE

#### Scenario: Stack overflow detection
- **WHEN** a reader enables stack protection
- **THEN** they understand CONFIG_STACK_SENTINEL and CONFIG_STACK_CANARIES

### Requirement: Memory protection (MPU/MMU)
The guide SHALL explain hardware memory protection.

#### Scenario: MPU configuration
- **WHEN** a reader enables MPU
- **THEN** they understand CONFIG_HW_STACK_PROTECTION and memory domains

#### Scenario: User mode
- **WHEN** a reader uses userspace
- **THEN** they understand CONFIG_USERSPACE, syscalls, and memory partitions

#### Scenario: Memory domains diagram
- **WHEN** viewing memory protection section
- **THEN** a mermaid diagram shows kernel, shared, and user memory regions

### Requirement: Static vs dynamic allocation guidance
The guide SHALL provide guidance on allocation strategy for embedded systems.

#### Scenario: Static allocation preference
- **WHEN** a reader designs memory usage
- **THEN** they understand why static allocation is preferred for determinism

#### Scenario: Dynamic allocation use cases
- **WHEN** dynamic allocation is needed
- **THEN** they understand appropriate scenarios (variable-size data, optional features)
