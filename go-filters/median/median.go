package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func filter(x []float64, size int) {
	values := make([]float64, size)
	var sum float64
	var index, count int

	for i := 0; i < len(x); i++ {
		avg := 0.0

		sum -= values[index]
		values[index] = x[i]
		sum += values[index]

		index++
		index %= size

		if count < size {
			count++
		}

		avg = sum / float64(count)

		x[i] = avg - x[i]
	}
}

func main() {
	input, err := os.Open("dataIR.txt")
	if err != nil {
		fmt.Print(err)
	}
	defer input.Close()

	var x []float64
	scanner := bufio.NewScanner(input)
	for scanner.Scan() {
		val, _ := strconv.ParseFloat(scanner.Text(), 64)
		x = append(x, val)
	}

	filter(x, 15)

	output, err := os.Create("dataIR_filtered.txt")
	if err != nil {
		fmt.Print(err)
	}
	defer output.Close()

	for _, val := range x {
		output.WriteString(fmt.Sprintln(val))
	}
}
