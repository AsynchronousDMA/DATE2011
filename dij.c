#include "scheduler.h"
#include "app.h"

#define N_NODES   25
#define Q_SIZE    4 * N_NODES

#define INFINITY  0xFFFF
#define UNDEFINED 0xFFFF

const uint8_t adj_matrix[50][50] = {
    {52, 58, 21, 41, 7, 51, 29, 90, 93, 38, 95, 26, 96, 72, 42, 92, 81, 14, 27, 85, 77, 20, 49, 27, 49, 90, 76, 82, 5, 86, 94, 10, 49, 93, 40, 98, 34, 2, 37, 92, 61, 27, 13, 16, 25, 88, 96, 50, 51, 52},
    {92, 53, 8, 66, 33, 29, 85, 6, 26, 91, 42, 42, 80, 18, 89, 35, 17, 86, 2, 99, 56, 93, 11, 79, 10, 37, 34, 46, 59, 18, 98, 83, 98, 15, 23, 29, 96, 41, 28, 40, 93, 42, 64, 50, 76, 90, 39, 53, 50, 7},
    {92, 18, 100, 87, 51, 97, 58, 34, 37, 26, 87, 13, 11, 90, 45, 80, 87, 61, 61, 17, 64, 72, 31, 2, 68, 78, 60, 63, 16, 92, 77, 94, 37, 53, 2, 4, 0, 31, 95, 100, 99, 79, 2, 78, 27, 6, 2, 97, 82, 90},
    {76, 95, 8, 90, 6, 19, 25, 11, 40, 49, 92, 58, 1, 33, 49, 80, 46, 30, 76, 73, 84, 68, 44, 86, 74, 79, 66, 9, 7, 59, 7, 83, 7, 74, 5, 50, 12, 82, 15, 41, 86, 93, 26, 96, 84, 45, 83, 34, 41, 3},
    {55, 31, 8, 10, 92, 99, 64, 27, 20, 28, 50, 12, 7, 89, 30, 46, 98, 69, 19, 47, 11, 59, 62, 83, 69, 39, 97, 10, 93, 54, 2, 59, 81, 53, 67, 20, 0, 91, 99, 77, 30, 72, 9, 93, 6, 76, 94, 14, 72, 59},
    {12, 3, 23, 16, 76, 76, 21, 83, 68, 92, 70, 28, 63, 30, 35, 16, 2, 50, 41, 80, 28, 9, 73, 14, 27, 80, 47, 1, 56, 70, 68, 48, 47, 71, 34, 61, 96, 4, 97, 31, 76, 77, 73, 54, 43, 75, 68, 9, 57, 64},
    {84, 33, 74, 5, 11, 23, 4, 35, 92, 78, 8, 45, 22, 82, 39, 21, 73, 53, 77, 63, 1, 7, 25, 22, 38, 25, 59, 66, 2, 94, 10, 78, 69, 6, 21, 16, 25, 29, 92, 51, 82, 31, 88, 63, 10, 13, 61, 8, 86, 84},
    {75, 10, 0, 77, 56, 78, 1, 7, 62, 29, 100, 6, 18, 92, 24, 15, 47, 9, 52, 4, 100, 92, 80, 23, 46, 75, 49, 3, 61, 77, 11, 15, 66, 14, 81, 91, 84, 58, 50, 11, 64, 2, 93, 89, 34, 82, 56, 38, 20, 80},
    {97, 89, 38, 27, 28, 85, 12, 28, 37, 26, 94, 25, 41, 4, 82, 40, 38, 59, 4, 51, 55, 44, 90, 61, 99, 43, 92, 77, 45, 83, 15, 66, 35, 78, 87, 46, 48, 47, 53, 89, 87, 14, 81, 50, 35, 46, 96, 3, 28, 23},
    {82, 38, 72, 97, 64, 46, 86, 97, 19, 28, 60, 34, 87, 63, 10, 29, 78, 52, 69, 0, 83, 46, 72, 64, 9, 44, 17, 41, 42, 69, 94, 3, 56, 5, 39, 27, 43, 93, 7, 79, 13, 60, 62, 46, 71, 21, 14, 47, 5, 22},
    {17, 52, 46, 2, 35, 61, 40, 7, 27, 58, 36, 59, 85, 80, 60, 57, 90, 70, 99, 9, 14, 31, 10, 26, 66, 81, 5, 4, 59, 80, 17, 59, 13, 89, 43, 30, 60, 33, 71, 53, 30, 13, 6, 92, 20, 11, 16, 82, 65, 95},
    {8, 75, 77, 51, 27, 75, 51, 95, 25, 35, 34, 70, 48, 27, 95, 7, 89, 40, 65, 98, 39, 86, 0, 75, 57, 67, 99, 75, 62, 97, 26, 48, 42, 96, 5, 24, 67, 97, 1, 23, 5, 87, 62, 53, 37, 38, 16, 48, 7, 37},
    {19, 76, 92, 87, 97, 20, 2, 5, 8, 73, 42, 56, 96, 44, 35, 67, 53, 32, 67, 35, 89, 41, 73, 38, 99, 25, 76, 100, 100, 45, 24, 93, 63, 73, 80, 91, 28, 37, 87, 50, 95, 7, 55, 42, 20, 58, 70, 68, 22, 43},
    {4, 5, 29, 34, 1, 53, 33, 14, 6, 15, 57, 13, 35, 80, 45, 92, 54, 47, 21, 54, 62, 73, 11, 3, 53, 82, 100, 83, 90, 73, 18, 3, 2, 66, 39, 70, 73, 12, 54, 57, 92, 7, 6, 67, 22, 59, 30, 96, 100, 82},
    {19, 43, 89, 17, 96, 75, 30, 18, 33, 89, 14, 72, 51, 78, 27, 83, 43, 38, 100, 22, 98, 28, 58, 48, 14, 59, 61, 82, 74, 60, 60, 12, 33, 67, 43, 80, 27, 45, 92, 88, 43, 93, 36, 56, 21, 64, 67, 25, 49, 13},
    {9, 38, 11, 0, 26, 72, 95, 74, 28, 74, 38, 35, 0, 32, 46, 74, 94, 48, 42, 30, 28, 83, 12, 26, 69, 61, 0, 8, 43, 32, 34, 94, 41, 56, 54, 13, 68, 69, 95, 22, 51, 69, 11, 83, 57, 82, 97, 80, 1, 2},
    {19, 19, 26, 82, 62, 75, 56, 91, 74, 92, 14, 75, 10, 8, 64, 83, 1, 57, 90, 17, 26, 35, 71, 27, 69, 7, 13, 38, 50, 11, 79, 15, 26, 79, 17, 94, 79, 9, 19, 57, 28, 82, 12, 33, 79, 61, 33, 40, 99, 36},
    {84, 67, 78, 84, 72, 86, 57, 88, 63, 38, 32, 66, 90, 7, 84, 78, 67, 80, 45, 3, 47, 86, 99, 4, 92, 25, 12, 67, 66, 82, 47, 8, 38, 25, 25, 49, 33, 24, 57, 39, 10, 12, 99, 71, 57, 33, 6, 41, 48, 95},
    {98, 73, 88, 18, 63, 24, 23, 51, 74, 8, 67, 49, 38, 2, 90, 25, 34, 61, 50, 15, 50, 51, 98, 60, 33, 59, 50, 61, 22, 78, 75, 75, 87, 80, 29, 76, 38, 26, 63, 52, 53, 84, 90, 60, 57, 58, 98, 13, 75, 36},
    {33, 75, 71, 2, 21, 7, 52, 15, 49, 26, 77, 71, 70, 31, 74, 53, 97, 59, 47, 71, 85, 14, 80, 19, 69, 94, 7, 30, 71, 46, 84, 82, 14, 20, 88, 87, 4, 97, 94, 1, 43, 90, 95, 22, 4, 59, 97, 9, 6, 13},
    {56, 52, 43, 6, 4, 56, 26, 94, 79, 71, 77, 86, 22, 7, 19, 14, 25, 36, 91, 20, 96, 68, 22, 21, 31, 72, 79, 22, 53, 95, 51, 9, 42, 9, 0, 0, 62, 100, 58, 77, 82, 80, 32, 71, 17, 88, 64, 57, 58, 35},
    {99, 35, 80, 80, 36, 46, 34, 76, 44, 29, 84, 79, 34, 16, 43, 56, 90, 35, 77, 83, 2, 95, 14, 10, 80, 87, 54, 48, 69, 77, 29, 4, 45, 3, 4, 4, 22, 93, 0, 42, 3, 3, 18, 29, 25, 24, 68, 28, 83, 42},
    {0, 88, 23, 11, 76, 54, 94, 16, 37, 66, 88, 5, 36, 47, 19, 94, 96, 30, 20, 57, 68, 12, 0, 62, 61, 54, 75, 70, 79, 30, 6, 36, 11, 24, 77, 25, 43, 83, 84, 16, 13, 44, 20, 11, 40, 20, 95, 28, 98, 43},
    {80, 88, 57, 45, 77, 35, 60, 38, 10, 58, 28, 5, 34, 89, 27, 1, 40, 84, 49, 15, 72, 93, 52, 53, 97, 35, 69, 20, 48, 20, 93, 97, 53, 45, 47, 21, 10, 91, 75, 70, 50, 14, 22, 81, 37, 28, 68, 35, 49, 49},
    {80, 92, 67, 91, 89, 63, 34, 5, 23, 80, 77, 23, 5, 81, 73, 17, 35, 79, 58, 7, 71, 86, 24, 2, 82, 41, 40, 24, 29, 42, 86, 59, 83, 22, 19, 87, 50, 37, 66, 38, 24, 94, 85, 87, 8, 97, 69, 19, 71, 37},
    {9, 67, 24, 29, 100, 60, 16, 84, 60, 26, 79, 59, 25, 45, 47, 29, 88, 97, 18, 33, 32, 88, 95, 53, 85, 98, 77, 32, 59, 3, 65, 0, 58, 57, 10, 13, 47, 39, 20, 28, 39, 75, 35, 87, 82, 53, 31, 17, 16, 11},
    {72, 16, 30, 46, 70, 52, 67, 91, 64, 67, 24, 88, 9, 46, 1, 72, 12, 60, 13, 78, 80, 90, 90, 32, 81, 88, 2, 7, 3, 62, 57, 43, 14, 51, 88, 54, 33, 58, 48, 76, 10, 76, 93, 53, 89, 8, 0, 3, 51, 95},
    {80, 75, 43, 67, 43, 80, 84, 1, 12, 63, 92, 6, 38, 93, 80, 48, 8, 53, 5, 47, 31, 63, 100, 87, 73, 61, 40, 18, 29, 42, 87, 5, 51, 20, 89, 33, 3, 55, 40, 65, 29, 2, 54, 67, 38, 31, 25, 29, 78, 3},
    {45, 24, 60, 87, 24, 45, 40, 92, 45, 80, 52, 3, 7, 12, 1, 93, 92, 81, 34, 10, 15, 42, 5, 72, 30, 54, 36, 22, 50, 48, 85, 59, 10, 74, 90, 88, 93, 94, 69, 65, 25, 84, 90, 43, 16, 37, 59, 52, 20, 53},
    {37, 68, 93, 69, 15, 17, 27, 8, 85, 51, 43, 64, 66, 10, 44, 90, 30, 31, 47, 70, 12, 8, 80, 83, 58, 64, 48, 78, 18, 11, 28, 88, 71, 20, 23, 82, 60, 70, 56, 78, 60, 5, 38, 22, 82, 47, 12, 52, 43, 87},
    {68, 18, 93, 49, 66, 47, 97, 84, 47, 79, 92, 96, 29, 82, 12, 8, 22, 59, 44, 14, 55, 97, 37, 30, 78, 59, 98, 7, 71, 83, 59, 6, 15, 30, 90, 13, 80, 0, 20, 70, 16, 68, 39, 26, 85, 37, 35, 46, 89, 15},
    {22, 29, 17, 30, 8, 100, 73, 53, 92, 70, 12, 24, 25, 99, 43, 76, 10, 45, 55, 41, 63, 67, 4, 86, 23, 70, 6, 87, 88, 18, 42, 97, 71, 45, 44, 49, 42, 30, 50, 11, 73, 46, 12, 1, 53, 70, 81, 0, 98, 70},
    {21, 96, 48, 3, 100, 64, 20, 75, 92, 12, 0, 49, 96, 30, 56, 71, 78, 27, 95, 22, 49, 0, 100, 15, 82, 43, 15, 8, 88, 87, 18, 90, 19, 31, 34, 49, 53, 19, 69, 85, 51, 60, 23, 20, 47, 45, 78, 73, 50, 94},
    {86, 78, 49, 90, 36, 41, 70, 43, 36, 94, 17, 36, 95, 87, 37, 10, 37, 88, 61, 42, 23, 92, 9, 72, 23, 46, 21, 27, 14, 5, 53, 2, 28, 84, 20, 33, 73, 10, 31, 9, 39, 22, 52, 90, 1, 100, 96, 88, 58, 4},
    {91, 9, 96, 64, 53, 37, 68, 17, 73, 43, 46, 3, 47, 46, 94, 84, 0, 4, 17, 22, 75, 53, 83, 64, 56, 3, 48, 49, 86, 6, 44, 77, 52, 15, 82, 65, 0, 6, 27, 66, 64, 42, 77, 60, 11, 24, 77, 73, 28, 5},
    {41, 7, 24, 87, 18, 73, 81, 66, 40, 36, 96, 58, 77, 50, 44, 13, 3, 73, 32, 33, 27, 34, 75, 22, 56, 56, 28, 89, 69, 90, 68, 100, 5, 19, 8, 10, 65, 36, 8, 61, 83, 31, 17, 56, 53, 59, 65, 87, 40, 36},
    {18, 54, 21, 100, 83, 42, 5, 29, 84, 79, 39, 96, 14, 58, 94, 58, 0, 84, 77, 97, 17, 1, 5, 50, 14, 29, 10, 49, 10, 69, 88, 59, 57, 40, 92, 25, 26, 73, 46, 84, 96, 22, 63, 62, 94, 16, 47, 5, 73, 96},
    {28, 22, 77, 73, 0, 94, 7, 88, 95, 85, 93, 70, 78, 32, 18, 40, 37, 85, 26, 85, 73, 28, 53, 35, 86, 85, 50, 34, 68, 7, 44, 76, 41, 40, 3, 2, 76, 53, 75, 40, 14, 6, 5, 77, 93, 72, 5, 22, 100, 34},
    {68, 0, 28, 86, 38, 47, 45, 45, 98, 18, 25, 77, 98, 69, 73, 43, 26, 48, 26, 37, 16, 3, 40, 82, 91, 15, 75, 0, 15, 89, 12, 80, 0, 5, 21, 66, 44, 80, 4, 31, 21, 90, 75, 8, 50, 68, 97, 24, 90, 62},
    {28, 13, 36, 18, 79, 84, 72, 79, 1, 63, 3, 39, 33, 85, 0, 31, 7, 31, 49, 7, 74, 85, 98, 21, 40, 99, 35, 63, 32, 52, 82, 79, 79, 54, 63, 21, 41, 17, 65, 96, 52, 75, 38, 51, 36, 60, 58, 60, 40, 33},
    {1, 33, 28, 29, 74, 28, 88, 28, 52, 96, 53, 83, 45, 99, 73, 69, 19, 67, 39, 100, 96, 73, 31, 61, 30, 76, 72, 33, 70, 65, 98, 59, 65, 52, 94, 90, 100, 40, 44, 29, 99, 69, 35, 83, 74, 99, 67, 80, 11, 44},
    {24, 52, 73, 41, 73, 89, 20, 50, 16, 33, 15, 0, 31, 30, 36, 85, 30, 33, 44, 73, 72, 41, 87, 66, 11, 21, 20, 11, 86, 63, 95, 34, 89, 30, 85, 91, 84, 43, 71, 10, 27, 72, 40, 5, 81, 71, 90, 20, 78, 85},
    {61, 7, 14, 80, 1, 84, 17, 88, 90, 46, 82, 8, 61, 64, 87, 56, 94, 76, 98, 75, 4, 38, 79, 21, 84, 29, 2, 40, 47, 94, 98, 6, 75, 5, 74, 34, 96, 57, 20, 80, 48, 85, 63, 36, 11, 18, 11, 79, 6, 35},
    {100, 11, 83, 86, 7, 11, 61, 47, 91, 16, 72, 71, 99, 39, 38, 0, 82, 4, 97, 6, 99, 52, 39, 54, 47, 34, 30, 59, 95, 9, 46, 58, 58, 10, 41, 44, 100, 3, 63, 19, 49, 78, 45, 88, 12, 23, 65, 92, 70, 26},
    {50, 57, 25, 95, 67, 25, 72, 24, 40, 32, 100, 20, 44, 71, 90, 87, 47, 55, 17, 20, 69, 13, 7, 9, 82, 78, 18, 31, 12, 77, 88, 60, 72, 49, 72, 32, 20, 54, 82, 87, 77, 25, 31, 35, 56, 32, 60, 99, 99, 19},
    {57, 40, 8, 74, 78, 86, 87, 19, 51, 32, 59, 99, 81, 70, 38, 34, 27, 83, 74, 92, 16, 18, 6, 65, 55, 13, 77, 34, 89, 43, 13, 97, 8, 83, 94, 18, 74, 37, 34, 91, 96, 35, 4, 32, 38, 54, 55, 3, 16, 43},
    {34, 19, 22, 8, 17, 55, 7, 19, 64, 31, 56, 93, 56, 57, 87, 87, 63, 82, 12, 66, 18, 45, 92, 57, 89, 24, 25, 64, 9, 51, 75, 0, 63, 8, 16, 21, 8, 95, 65, 14, 63, 59, 1, 67, 8, 66, 89, 5, 66, 69},
    {30, 40, 48, 53, 88, 37, 95, 68, 52, 96, 60, 84, 11, 18, 41, 57, 33, 94, 97, 19, 31, 11, 96, 18, 11, 15, 81, 19, 76, 82, 67, 83, 94, 97, 7, 25, 20, 42, 15, 4, 17, 35, 50, 21, 88, 38, 9, 98, 72, 87},
    {13, 35, 69, 90, 8, 69, 28, 28, 38, 32, 84, 12, 67, 90, 17, 66, 32, 36, 82, 44, 47, 70, 47, 33, 38, 59, 66, 63, 36, 60, 51, 52, 43, 81, 12, 36, 77, 49, 36, 95, 45, 43, 100, 19, 1, 8, 40, 48, 69, 6},
    {68, 59, 75, 94, 67, 73, 35, 24, 14, 21, 94, 19, 64, 24, 29, 35, 31, 3, 40, 62, 20, 20, 91, 88, 68, 58, 44, 13, 55, 10, 88, 69, 64, 38, 15, 32, 20, 55, 21, 78, 32, 84, 38, 73, 1, 72, 45, 64, 95, 56}
};


typedef struct {
	uint16_t dist;
	uint16_t prev;
} node_t;

typedef struct {
	uint16_t node;
	uint16_t dist;
	uint16_t prev;
} queue_t;

/**
 * 1. TEB declaration here.
 */
TEB(task_init);
TEB(task_init_list);
TEB(task_select_nearest_node);
TEB(task_find_shorter_path);
TEB(task_deq);

/**
 * 2. Shared variable declaration here. (714bytes)
 */
__shared(
#if !LAYOUT
    // DMAflagStart;

 	  uint16_t multiple(deq_idx);
		uint8_t flag1;
		node_t multarray(node_list,N_NODES);
		uint8_t flag3;
		uint16_t multiple(enq_idx);
		uint8_t flag2;
		uint16_t multiple(node_idx);
		uint8_t flag4;
		uint16_t multiple(src_node);
    queue_t multiple(nearest_node);
		queue_t multarray(queue,Q_SIZE); 
		//uint8_t index;
#else
	  uint16_t multiple(deq_idx);
		uint8_t flag1;
		node_t multarray(node_list,N_NODES);
		uint8_t flag3;
		uint16_t multiple(enq_idx);
		uint8_t flag2;
		uint16_t multiple(node_idx);
		uint8_t flag4;
		uint16_t multiple(src_node);
    queue_t multiple(nearest_node);
		queue_t multarray(queue,Q_SIZE);
#endif
)

TEB(task_init)//0
{
    
    __SET(deq_idx) = 0;
    __SET(enq_idx) = 1;

    // Enqueue.
	__SET(src_node) = 0;
	//__SET_FLAG(flag1);


	//NEXT(1);
	 uint16_t i, sn;
    //__SET_FLAG(DMAflag1);
   for (i = 0; i < N_NODES; i++) {
    	__SET(node_list[i].dist) = INFINITY;
    	__SET(node_list[i].prev) = UNDEFINED;
   }

   sn = __GET(src_node);
   __SET(node_list[sn].dist) = 0;
	 __SET(node_list[sn].prev) = UNDEFINED;

   sn++;
   // Test nodes 0, 1, 2, 3.
   if (sn < 4) {
		//__SET_FLAG(flag1);
    	__SET(src_node) = sn;
   } else {
			//__SET_FLAG(flag1);
    	__SET(src_node) = 0;
   }

    NEXT(2);
	
}


TEB(task_init_list)//1
{
    uint16_t i, sn;
    //__SET_FLAG(DMAflag1);
    for (i = 0; i < N_NODES; i++) {
    	__SET(node_list[i].dist) = INFINITY;
    	__SET(node_list[i].prev) = UNDEFINED;
    }

    sn = __GET(src_node);
    __SET(node_list[sn].dist) = 0;
    __SET(node_list[sn].prev) = UNDEFINED;

    sn++;
    if (sn < 4) {
		__SET_FLAG(flag1);
    	__SET(src_node) = sn;
    } else {
		__SET_FLAG(flag1);
    	__SET(src_node) = 0;
    }

    NEXT(2);
}


TEB(task_select_nearest_node)//2
{
	
	uint16_t i = __GET(deq_idx);

	//queue_t queue[Q_SIZE];
	__SET(queue[0].node) = __GET(src_node);
	__SET(queue[0].dist) = 0;
	__SET(queue[0].prev) = UNDEFINED;

	if (__GET(enq_idx) != __GET(deq_idx)) {
		
		// Dequeue nearest node.    
		__SET(nearest_node.node) = __GET(queue[__GET(deq_idx)].node);
		__SET(nearest_node.dist) = __GET(queue[__GET(deq_idx)].dist);
		__SET(nearest_node.prev) = __GET(queue[__GET(deq_idx)].prev);
		



		__SET(node_idx) = 0;	
		if (__GET(deq_idx) + 1 < Q_SIZE) {
			__SET_FLAG(flag1);
			__SET(deq_idx)++;
		} else {
			__SET_FLAG(flag1);
			__SET(deq_idx) = 0;
		}
		NEXT(3);
	}
	else {
		NEXT(0);
	}
}



TEB(task_find_shorter_path)//3
{
	uint16_t cost, node, dist, nearest_dist, i;
	i = __GET(node_idx);
	
	node_t node_temp[N_NODES];
	for(int j = 0; j < N_NODES; j++)
	{
		node_temp[j].dist =  __GET(node_list[j].dist);
		node_temp[j].prev =  __GET(node_list[j].prev);
	}
	for(;i < N_NODES; i++)
	{
		node = __GET(nearest_node.node);
		cost = adj_matrix[node][i];
	


		if (cost != INFINITY) {
			nearest_dist = __GET(nearest_node.dist);
			dist = node_temp[i].dist;
		if (dist == INFINITY || dist > (cost + nearest_dist)) {
		
				node_temp[i].dist = nearest_dist + cost;
				node_temp[i].prev = node;

			// Enqueue.
				uint16_t j = __GET(enq_idx);
				if (j == (__GET(deq_idx) - 1)) {
					//LOG("QUEUE IS FULL!");
				}
					__SET(queue[j].node) = i;
					__SET(queue[j].dist) = nearest_dist + cost;
					__SET(queue[j].prev) = node;
				j++;
				__SET_FLAG(flag2);
				if (j < Q_SIZE) {
				__SET(enq_idx) = j;
				} else {
				__SET(enq_idx) = 0;
				}
			}
		}
	}
	__SET_FLAG(flag3);
	for(int j = 0; j < N_NODES; j++)
	{
		__SET(node_list[j].dist) = node_temp[j].dist;
		__SET(node_list[j].prev) = node_temp[j].prev;
	}
	__SET_FLAG(flag4);
	__SET(node_idx) = N_NODES;
	NEXT(2);
	
}


TEB(task_deq)//4
{
	
		if (__GET(deq_idx) + 1 < Q_SIZE) {
			__SET_FLAG(flag1);
			__SET(deq_idx)++;
		} else {
			__SET_FLAG(flag1);
			__SET(deq_idx) = 0;
		}
		NEXT(3);
}

/**
 * 0. Benchmark app Init here.
 */
void dijkstra_init()
{
    TASK(1,task_init);

    TEB_INIT(task_init, 0);       //0
    TEB_INIT( task_init_list,             1); //1
    TEB_INIT( task_select_nearest_node,   2); //2
    TEB_INIT(task_find_shorter_path,      3);    //3
    TEB_INIT( task_deq,                   4);  //4
		FLAG_DECLEAR(0,__GET_OFFSET(flag1));
		FLAG_DECLEAR(1,__GET_OFFSET(flag2));
		FLAG_DECLEAR(2,__GET_OFFSET(flag3));
		FLAG_DECLEAR(3,__GET_OFFSET(flag4));
	
	
	
}
