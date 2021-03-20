/*

� ��� ���� ��������� ��������� � ������, ������� ����������� ��� C. ������� � ������� i ����� ��� xi. ���������� ����� ��������� ������� ���������, ������� ����� ������ � �������. ��� ������ ��������� ����������, ���� ���������� ���� �� ���� �������, ������� ������� � ���� �� ������� � �� ������� � ������.

������� ������
� ������ ������ ����� �������� ����� ����� n � ���������� ��������� (1\N{LESS-THAN OR EQUAL TO}n\N{LESS-THAN OR EQUAL TO}30). �� ������ ������ �������� n ����� ����� xi (1\N{LESS-THAN OR EQUAL TO}xi\N{LESS-THAN OR EQUAL TO}109). � ������ ������ �������� ����� ����� C � ����������� ������� (0\N{LESS-THAN OR EQUAL TO}C\N{LESS-THAN OR EQUAL TO}109).

�������� ������
�������� ������������ ����� ����� � ������� ����� ��������.

�������
������� ������
1
1
1
�������� ������
2
������� ������
1
1
2
�������� ������
2
������� ������
2
2 2
1
�������� ������
1
������� ������
2
1 1
2
�������� ������
4
������� ������
2
1 1
1
�������� ������
3
������� ������
30
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
30
�������� ������
1073741824

*/


#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>


unsigned countSets(unsigned maxIndex, unsigned maxWeight, unsigned *seq) {
    //if (!maxIndex)
    //    return 1;

    if (maxIndex == 1)
        return (seq[0] <= maxWeight) + 1;  // Optimization

    //if (!maxWeight)
    //    return 1;

    unsigned result = countSets(maxIndex - 1, maxWeight, seq);

    if (seq[maxIndex - 1] < maxWeight)
        result += countSets(maxIndex - 1, maxWeight - seq[maxIndex - 1], seq);
    else if (seq[maxIndex - 1] == maxWeight)  // Optimization
        result++;

    return result;
}


int main() {
    unsigned n = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    unsigned *seq = new unsigned[n];

    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%u", &seq[i]);
        assert(res == 1);
    }

    unsigned c = 0;
    res = scanf("%u", &c);
    assert(res == 1);

    printf("%u\n", countSets(n, c, seq));

    delete[] seq;

    return 0;
}


/*

A friend hinted me a much better idea than the original meet-in-the-middle dynamics:
Regular dynamics, but implemented without a dp array, due to how big the weights can be.
Essentially, with an N as small as 30, we can afford two recursive calls without caching - so voila!
dp[maxIndex][maxWeight] holds the number of combinations of items [0, maxIndex) weighting [0, maxWeight]
(If I were to implement it as an actual array, I could have got rid of the first dimension and do everything in-place)

If not for others' success, I would have thought this to be impossible - but wow, 1996/2000...

*/
