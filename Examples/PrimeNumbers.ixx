#include <cstdint>
#include <iostream>
#include <syncstream>

export module PrimeNumbers;

namespace PrimeNumbers
{
    static std::atomic<int> totalPrimesFound;

    export bool IsPrime(uint64_t number)
    {
        //Purposely inefficient prime finding algorithm
        uint64_t temp = 0;
        for (uint64_t i = 2; i < number-1; ++i)
        {
            if (number % i == 0)
            {
                return false;
            }
        }
        return true;
    }

    export uint64_t Find(uint32_t nthPrime)
    {
        uint64_t prime = 0;

        uint64_t checkedNum = 2;
        uint64_t numPrimesFound = 0;
        while (prime == 0)
        {
            if (IsPrime(checkedNum))
            {
                ++numPrimesFound;
            }

            if (numPrimesFound == nthPrime)
            {
                prime = checkedNum;
            }

            ++checkedNum;
        }

        totalPrimesFound = totalPrimesFound + 1;
        std::osyncstream{ std::cout } << "Prime found: " << prime << " number: " << totalPrimesFound << std::endl;

        return prime;
    }
}