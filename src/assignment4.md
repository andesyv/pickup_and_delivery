# Results
| Call_7_Vehicle_3          |                   |                |                 |              |
|---------------------------|-------------------|----------------|-----------------|--------------|
|                           | Average objective | Best objective | Improvement (%) | Running time |
| Random Search             | 1720310           | 1477429        | 55.04           | 0.1s       |
| Local Search              | 1608860           | 1476444        | 55.07           | 0.3s        |
| Simulated Annealing (old) | 1476440           | 1476444        | 55.07           | 0.3s      |
| Simulated Annealing (new) | 1482862           | 1476444        | 55.07           | 0.4s     |
|                           |                   |                |                 |              |
| Call_18_Vehicle_5         |                   |                |                 |              |
|                           | Average objective | Best objective | Improvement (%) | Running time |
| Random Search             | 8761490           | 8761492        | 0               | 0.2s      |
| Local Search              | 3193100           | 2927103        | 66.59           | 1.9s     |
| Simulated Annealing (old) | 3053050           | 2702224        | 69.16           | 1.6s     |
| Simulated Annealing (new) | 3564640           | 2817694        | 67.84           | 1.7s    |
|                           |                   |                |                 |              |
| Call_035_Vehicle_07       |                   |                |                 |              |
|                           | Average objective | Best objective | Improvement (%) | Running time |
| Random Search             | 18322200          | 18322178       | 0               | 0.2s      |
| Local Search              | 8278980           | 7129880        | 61.09           | 5.1s     |
| Simulated Annealing (old) | 8016300           | 7347446        | 59.90           | 5.1s     |
| Simulated Annealing (new) | 8575118           | 7697373        | 57.99           | 3.9s    |
|                           |                   |                |                 |              |
| Call_080_Vehicle_20       |                   |                |                 |              |
|                           | Average objective | Best objective | Improvement (%) | Running time |
| Random Search             | 42211400          | 42211425       | 0               | 0.6s        |
| Local Search              | 19084400          | 17682737       | 58.11           | 33.1s    |
| Simulated Annealing (old) | 19260700          | 18572357       | 56.00           | 33.6s    |
| Simulated Annealing (new) | 20996094          | 19161770       | 54.61           | 30.7s   |
|                           |                   |                |                 |              |
| Call_130_Vehicle_40       |                   |                |                 |              |
|                           | Average objective | Best objective | Improvement (%) | Running time |
| Random Search             | 75446700          | 75446687       | 0               | 1.1s     |
| Local Search              | 32781400          | 31228964       | 58.61           | 101.8s     |
| Simulated Annealing (old) | 33371800          | 31800858       | 57.85           | 103.1s     |
| Simulated Annealing (new) | 35818796          | 33527884       | 55.56           | 112.0s  |

# Solutions
## Old best
| Filename | Old best solutions |
| -------- | ------------------ |
| Call_7_Vehicle_3 | 3 3 0 7 1 7 1 0 5 5 6 6 0 2 4 2 4 |
| Call_18_Vehicle_5 | 18 11 1 11 1 18 0 15 15 16 16 10 10 9 9 0 4 14 14 4 3 3 0 6 6 8 7 7 8 2 2 0 12 5 5 12 0 17 17 13 13 |
| Call_035_Vehicle_07 | 12 24 12 1 24 1 0 23 23 22 22 33 33 0 5 17 17 30 5 8 8 30 31 31 0 7 19 7 18 19 18 10 10 27 27 0 6 34 13 13 34 6 32 32 26 26 0 25 25 20 2 2 20 0 28 28 35 29 29 35 14 14 0 21 21 3 3 9 15 16 15 11 11 4 4 16 9 |
| Call_080_Vehicle_20 | 74 74 75 75 0 34 34 24 31 31 24 0 66 66 7 13 13 7 0 47 47 40 40 73 73 0 53 53 30 30 19 38 38 19 0 37 61 61 5 5 37 64 64 0 29 11 29 11 10 10 0 72 12 12 72 41 41 43 43 0 33 33 42 18 42 6 18 6 0 67 59 59 67 39 39 0 55 54 55 28 28 54 65 65 0 57 57 70 21 21 70 0 45 8 8 45 0 4 4 25 25 0 69 69 14 44 14 44 0 49 49 35 35 0 46 46 15 15 0 16 16 56 56 76 1 76 1 0 60 23 60 23 0 3 3 0 48 17 26 71 20 9 51 50 78 58 50 32 32 20 80 79 79 27 22 27 36 9 52 26 36 71 63 77 22 2 78 63 68 51 48 58 62 68 52 62 17 2 77 80 |
| Call_130_Vehicle_40 | 68 68 81 124 124 81 0 26 83 26 83 53 53 0 64 64 0 117 117 24 24 0 91 91 38 38 0 84 84 60 60 106 106 0 74 74 0 52 52 105 105 4 4 0 82 82 2 2 0 58 58 39 39 0 28 28 61 61 0 18 116 116 18 0 114 114 10 10 0 122 29 122 29 129 129 0 120 120 93 93 5 5 0 59 59 100 100 0 73 73 98 98 0 108 42 42 108 0 41 14 41 79 79 14 0 37 128 37 130 130 128 0 43 55 55 43 0 34 34 92 92 0 102 89 102 89 44 44 45 45 0 101 101 119 119 88 88 0 77 77 67 95 67 95 0 96 96 78 78 85 85 0 107 107 70 70 0 121 121 75 75 0 97 35 35 97 0 66 66 20 20 0 87 87 3 3 0 32 32 125 125 113 113 0 111 22 22 111 36 8 36 8 0 16 31 16 31 0 90 90 54 54 27 27 0 123 123 99 99 0 40 46 40 46 0 57 80 57 80 0 7 25 25 7 0 50 50 110 1 1 110 0 51 104 115 51 86 56 56 63 11 11 48 112 115 112 94 94 13 13 72 118 6 118 21 126 47 12 72 47 103 71 71 103 15 15 65 21 65 69 17 104 12 48 62 62 23 19 23 126 69 109 6 109 86 30 30 17 76 76 33 33 49 63 49 9 9 127 19 127 |

## New best
| Filename | New best solutions |
| -------- | ------------------ |
| Call_7_Vehicle_3 | 3 3 0 7 1 7 1 0 5 5 6 6 0 2 4 4 2 |
| Call_18_Vehicle_5 | 12 11 14 11 10 12 10 9 14 9 0 13 4 13 7 4 3 7 3 0 6 16 1 16 6 1 0 8 8 2 2 0 18 5 5 18 0 17 15 17 15 |
| Call_035_Vehicle_07 | 21 21 35 8 8 35 20 20 0 23 23 29 11 11 29 2 2 0 19 18 18 19 14 24 14 1 31 24 1 31 0 25 25 30 30 0 3 3 10 33 10 27 33 27 26 26 0 7 5 13 17 17 5 13 7 0 28 28 12 12 0 22 15 9 34 4 4 9 34 16 15 16 6 22 32 32 6 |
| Call_080_Vehicle_20 | 70 21 21 70 0 50 50 61 61 19 19 0 66 66 24 13 24 13 43 43 0 57 55 55 27 27 57 41 41 0 73 73 0 72 14 14 72 65 65 8 8 0 74 7 7 74 0 26 26 0 4 4 51 51 0 53 53 30 30 0 33 33 60 60 0 34 34 12 5 31 12 18 5 31 18 0 49 29 49 29 10 10 0 46 46 0 48 37 48 42 42 6 35 35 37 6 0 3 3 44 44 15 15 0 80 80 0 16 16 56 56 75 75 25 25 0 52 52 64 38 1 38 64 1 0 20 59 20 54 28 28 54 23 23 59 0 45 63 78 71 77 32 62 17 58 36 39 36 2 69 79 45 68 40 77 69 11 71 2 47 76 67 39 76 67 58 62 32 68 17 78 22 11 9 9 47 40 22 79 63 |
| Call_130_Vehicle_40 | 61 61 0 75 75 0 107 107 0 39 31 39 1 31 1 0 37 37 80 80 36 36 0 2 2 100 100 0 108 114 108 114 0 66 12 66 12 0 128 29 81 81 29 128 0 120 120 52 52 0 33 33 59 72 59 72 0 34 34 92 92 20 20 0 27 40 40 27 0 57 90 90 57 0 122 35 122 35 0 104 104 116 116 0 64 64 0 6 38 5 5 6 38 0 65 65 112 19 112 19 0 94 94 106 106 0 84 117 117 84 0 45 45 0 14 4 4 14 0 73 18 18 73 113 113 0 7 83 16 7 83 16 44 44 3 3 0 22 78 22 78 0 26 26 56 56 10 10 0 25 25 87 87 53 53 0 54 55 54 55 0 30 98 30 23 98 23 0 102 126 102 105 126 105 8 8 0 49 21 21 41 49 129 41 129 0 82 82 69 69 95 95 86 86 85 85 0 74 96 96 74 0 123 123 0 118 43 118 43 0 28 77 28 77 110 110 0 111 60 60 111 0 47 47 127 130 127 130 0 24 124 124 24 0 15 15 32 62 97 67 9 67 62 115 70 103 115 109 58 11 91 32 99 70 76 11 17 101 63 50 119 68 9 13 97 119 79 13 88 17 89 91 71 109 76 68 71 99 125 51 103 125 101 121 79 89 48 46 42 63 88 51 50 58 121 93 42 48 93 46 |

# Operator description
## First operator (Feasible reinsert)
For this operator I wanted to do the same simple operation from 1-reinsert, but attempt do it in a way such that the resulting solution is (hopefully) feasible. I could not get the operator to ensure feasibility, but I could atleast do some simple logic to make a feasible solution more likely.

### Vehicle available calls
I choose a random call from a random vehicle, but when reinserting I only reinsert the call into a vehicle that can handle the call.

### Vehicle weight ratio
It would be expensive to simulate all possible call order combinations to get the least weight, so I simplify it by instead finding the current used capacity of all vehicles and divide that on the maximum capacity of the vehicle to get a ratio. I then filter out every vehicle that has succeeded a ratio of 1 (overfull vehicles), and I choose the lowest ratio to reinsert into.

Since this operator "constructs" a solution, I think it mostly intensifies the search.

## Second operator (time-window 2-exchange)
This operator works more like a reordering. I choose a random car with a pair of calls, I find all possible configurations of the pair (6 different configurations), and then I reorder the pair based on the first feasible configuration.

This operator modifies a similar solution towards more feasibility, so this also just intensifies the search.

## Third operator (dummy backinsert)
This operator takes a random car and either takes a random call from the car and moves it to the dummy car, or swaps a pair of random calls from the car and the dummy.

This operator kind of backtraces into a (most likely) feasible but still probably worse solution, so I think this operator diversifies the search.

## Fourth operator (concurrent shuffle)
This operator uses multi-threading and the 1-reinsert operator to simultaneously check multiple different solutions and chooses the best one. It was however implemented terribly slow, so I don't currently use it.

This operator diversifies the search.
