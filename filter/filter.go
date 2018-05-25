package main

import (
	"bufio"
	"fmt"
	"math"
	"os"
	"strconv"
)

func threshold(data []float64, maxDiff float64) {
	for i := 1; i < len(data); i++ {
		if math.Abs(data[i]-data[i-1]) > maxDiff {
			data[i] = data[i-1]
		}
	}
}

func dc(data []float64, alpha float64) {
	w := make([]float64, len(data))

	for i := 1; i < len(data); i++ {
		w[i] = float64(data[i]) + alpha*w[i-1]
		data[i] = w[i] - w[i-1]
	}
}

func meanMedian(data []float64, size int) {
	values := make([]float64, size)
	var sum float64
	var index, count int

	for i := 0; i < len(data); i++ {
		var avg float64

		sum -= values[index]
		values[index] = data[i]
		sum += values[index]

		index++
		index %= size

		if count < size {
			count++
		}

		avg = sum / float64(count)

		data[i] = avg - data[i]
	}
}

func butterworth(data []float64) {
	v := make([]float64, 2)

	for i := 0; i < len(data); i++ {
		v[0] = v[1]
		v[1] = (2.452372752527856026e-1 * data[i]) + (0.50952544949442879485 * v[0])

		data[i] = v[0] + v[1]
	}
}

func max(data []float64) float64 {
	m := data[100]
	
	for i := 100; i < len(data); i++ {
		if data[i] > m {
			m = data[i]
		}
	}
	
	return m
}

func heartrate(data []float64) float64 {
	peak := false
	beats := 0
	threshold := max(data) / 3.4
	fmt.Println("Threshold:", threshold)
	
	for i := 100; i < len(data); i++ {
		if !peak && data[i] > threshold {
			peak = true
			beats++
			fmt.Println(i)
		}

		if peak && data[i] <= threshold {
			peak = false
		}
	}

	fmt.Println("Beats:", beats)
	
	return float64(beats) * 6000.0 / float64(len(data) - 100)
}

func main() {
	input, err := os.Open(os.Args[1])
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
	}
	defer input.Close()

	var data []float64
	scanner := bufio.NewScanner(input)
	for scanner.Scan() {
		val, _ := strconv.ParseFloat(scanner.Text(), 64)
		data = append(data, val)
	}
	
	fmt.Println("BPM:", heartrate(data))
}
