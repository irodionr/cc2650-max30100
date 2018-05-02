package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func filter(x []float64) {
	w := make([]float64, len(x))

	for i := 1; i < len(x); i++ {
		w[i] = float64(x[i]) + 0.95*w[i-1]
		x[i] = w[i] - w[i-1]
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

	filter(x)

	output, err := os.Create("dataIR_filtered.txt")
	if err != nil {
		fmt.Print(err)
	}
	defer output.Close()

	for _, val := range x {
		output.WriteString(fmt.Sprintln(val))
	}
}
