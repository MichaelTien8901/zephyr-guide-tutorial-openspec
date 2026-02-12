/*
 * Memory Management Example
 *
 * Demonstrates k_malloc/k_free, k_heap, and memory slabs
 * for different allocation strategies.
 */

#include <zephyr/kernel.h>
#include <string.h>

/* ---- k_heap example ---- */

/* Define a dedicated heap (1024 bytes) */
K_HEAP_DEFINE(my_heap, 1024);

static void demo_k_heap(void)
{
	printk("\n--- k_heap Demo ---\n");

	/* Allocate from dedicated heap */
	void *buf1 = k_heap_alloc(&my_heap, 64, K_NO_WAIT);
	void *buf2 = k_heap_alloc(&my_heap, 128, K_NO_WAIT);
	void *buf3 = k_heap_alloc(&my_heap, 256, K_NO_WAIT);

	if (buf1) {
		printk("Allocated 64 bytes at %p\n", buf1);
		memset(buf1, 0xAA, 64);
	}

	if (buf2) {
		printk("Allocated 128 bytes at %p\n", buf2);
		memset(buf2, 0xBB, 128);
	}

	if (buf3) {
		printk("Allocated 256 bytes at %p\n", buf3);
		memset(buf3, 0xCC, 256);
	}

	/* Try an allocation that should fail (heap is ~1024 bytes total) */
	void *buf4 = k_heap_alloc(&my_heap, 800, K_NO_WAIT);
	if (buf4 == NULL) {
		printk("Allocation of 800 bytes failed (expected - heap full)\n");
	}

	/* Free in different order to demonstrate fragmentation handling */
	k_heap_free(&my_heap, buf2);
	printk("Freed 128-byte block\n");

	k_heap_free(&my_heap, buf1);
	printk("Freed 64-byte block\n");

	k_heap_free(&my_heap, buf3);
	printk("Freed 256-byte block\n");

	printk("k_heap demo complete\n");
}

/* ---- Memory slab example ---- */

/* Define a slab: 32-byte blocks, 8 blocks, 4-byte alignment */
K_MEM_SLAB_DEFINE(my_slab, 32, 8, 4);

struct sensor_data {
	uint32_t timestamp;
	int16_t temperature;
	int16_t humidity;
	uint8_t channel;
	uint8_t reserved[3];
};

static void demo_mem_slab(void)
{
	printk("\n--- Memory Slab Demo ---\n");
	printk("Slab: %u blocks of 32 bytes (%u free)\n",
	       8, k_mem_slab_num_free_get(&my_slab));

	void *blocks[8];
	int allocated = 0;

	/* Allocate all blocks */
	for (int i = 0; i < 8; i++) {
		int ret = k_mem_slab_alloc(&my_slab, &blocks[i], K_NO_WAIT);
		if (ret == 0) {
			struct sensor_data *data = blocks[i];
			data->timestamp = k_uptime_get_32();
			data->temperature = 2500 + i * 10;
			data->humidity = 600 - i * 5;
			data->channel = i;
			allocated++;
			printk("Block %d: temp=%d.%d C, hum=%d.%d%%\n",
			       i, data->temperature / 100,
			       data->temperature % 100,
			       data->humidity / 10,
			       data->humidity % 10);
		}
	}

	printk("Allocated %d blocks (%u free)\n",
	       allocated, k_mem_slab_num_free_get(&my_slab));

	/* Try one more - should fail */
	void *extra;
	int ret = k_mem_slab_alloc(&my_slab, &extra, K_NO_WAIT);
	if (ret != 0) {
		printk("9th allocation failed (expected - slab full)\n");
	}

	/* Free all blocks */
	for (int i = 0; i < allocated; i++) {
		k_mem_slab_free(&my_slab, blocks[i]);
	}
	printk("Freed all blocks (%u free)\n",
	       k_mem_slab_num_free_get(&my_slab));
}

/* ---- System heap (k_malloc/k_free) example ---- */

static void demo_system_heap(void)
{
	printk("\n--- System Heap (k_malloc/k_free) Demo ---\n");
	printk("CONFIG_HEAP_MEM_POOL_SIZE = 4096\n");

	/* Allocate a string buffer */
	char *msg = k_malloc(64);
	if (msg != NULL) {
		snprintf(msg, 64, "Hello from k_malloc on %s!", CONFIG_BOARD);
		printk("Message: %s\n", msg);
		k_free(msg);
		printk("Freed string buffer\n");
	} else {
		printk("k_malloc failed!\n");
	}

	/* Allocate an array of integers */
	int *data = k_malloc(10 * sizeof(int));
	if (data != NULL) {
		for (int i = 0; i < 10; i++) {
			data[i] = i * i;
		}
		printk("Array: ");
		for (int i = 0; i < 10; i++) {
			printk("%d ", data[i]);
		}
		printk("\n");
		k_free(data);
		printk("Freed array\n");
	}
}

int main(void)
{
	printk("\n");
	printk("Memory Management Example\n");
	printk("=========================\n");

	/* 1. System heap via k_malloc/k_free */
	demo_system_heap();

	/* 2. Dedicated k_heap */
	demo_k_heap();

	/* 3. Fixed-size memory slabs */
	demo_mem_slab();

	printk("\nAll memory demos complete.\n");

	return 0;
}
