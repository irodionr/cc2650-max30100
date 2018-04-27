package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func filter(x []float64) {
	v := make([]float64, 2)

	for i := 0; i < len(x); i++ {
		v[0] = v[1]
		v[1] = (2.452372752527856026e-1 * x[i]) + (0.50952544949442879485 * v[0])

		x[i] = v[0] + v[1]
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
