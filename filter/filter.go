package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func threshold(IR []float64) {
	for i := 1; i < len(IR); i++ {
		if IR[i] < 41000 || IR[i] > 44000 {
			IR[i] = IR[i-1]
		}
	}
}

func dc(IR []float64, alpha float64) {
	w := make([]float64, len(IR))

	for i := 1; i < len(IR); i++ {
		w[i] = float64(IR[i]) + alpha*w[i-1]
		IR[i] = w[i] - w[i-1]
	}
}

func median(IR []float64, size int) {
	values := make([]float64, size)
	var sum float64
	var index, count int

	for i := 0; i < len(IR); i++ {
		var avg float64

		sum -= values[index]
		values[index] = IR[i]
		sum += values[index]

		index++
		index %= size

		if count < size {
			count++
		}

		avg = sum / float64(count)

		IR[i] = avg - IR[i]
	}
}

func butterworth(IR []float64) {
	v := make([]float64, 2)

	for i := 0; i < len(IR); i++ {
		v[0] = v[1]
		v[1] = (2.452372752527856026e-1 * IR[i]) + (0.50952544949442879485 * v[0])

		IR[i] = v[0] + v[1]
	}
}

func heartrate(IR []float64) float64 {
	peak := false
	beats := 0

	for i := 0; i < len(IR); i++ {
		if !peak && IR[i] > 176.0 {
			peak = true
			beats++
		}

		if peak && IR[i] <= 176.0 {
			peak = false
		}
	}

	return float64(beats) * 6000.0 / float64(len(IR))
}

func main() {
	input, err := os.Open("dataIR.txt")
	if err != nil {
		fmt.Print(err)
	}
	defer input.Close()

	var IR []float64
	scanner := bufio.NewScanner(input)
	for scanner.Scan() {
		val, _ := strconv.ParseFloat(scanner.Text(), 64)
		IR = append(IR, val)
	}

	threshold(IR)
	dc(IR, 0.95)
	median(IR, 15)
	butterworth(IR)

	fmt.Println(heartrate(IR))

	output, err := os.Create("dataIR_heartrate.txt")
	if err != nil {
		fmt.Print(err)
	}
	defer output.Close()

	for _, val := range IR {
		output.WriteString(fmt.Sprintln(val))
	}
}
