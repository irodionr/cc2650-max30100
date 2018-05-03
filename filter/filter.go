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

func heartrate(data []float64) float64 {
	peak := false
	beats := 0

	for i := 0; i < len(data); i++ {
		if !peak && data[i] > 450.0 {
			peak = true
			beats++
		}

		if peak && data[i] <= 450.0 {
			peak = false
		}
	}

	return float64(beats) * 6000.0 / float64(len(data))
}

func main() {
	if len(os.Args) < 3 {
		fmt.Fprintln(os.Stderr, "Usage: -filter filename [arguments]")
	} else {
		input, err := os.Open(os.Args[2])
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

		switch os.Args[1] {
		case "-t":
			if len(os.Args) != 4 {
				fmt.Fprintln(os.Stderr, "Usage: -t filename maxDiff")
			} else {
				arg, _ := strconv.ParseFloat(os.Args[3], 64)
				threshold(data, arg)
			}
		case "-dc":
			if len(os.Args) != 4 {
				fmt.Fprintln(os.Stderr, "Usage: -dc filename alpha")
			} else {
				arg, _ := strconv.ParseFloat(os.Args[3], 64)
				dc(data, arg)
			}
		case "-mm":
			if len(os.Args) != 4 {
				fmt.Fprintln(os.Stderr, "Usage: -mm filename size")
			} else {
				arg, _ := strconv.Atoi(os.Args[3])
				meanMedian(data, arg)
			}
		case "-b":
			if len(os.Args) != 3 {
				fmt.Fprintln(os.Stderr, "Usage: -b filename")
			} else {
				butterworth(data)
			}
		default:
			fmt.Fprintln(os.Stderr, "Filters:")
			fmt.Fprintln(os.Stderr, "-t - threshold")
			fmt.Fprintln(os.Stderr, "-dc - DC offset")
			fmt.Fprintln(os.Stderr, "-mm - mean median")
			fmt.Fprintln(os.Stderr, "-b - butterworth")
			fmt.Fprintln(os.Stderr, "-m - median")

			os.Exit(1)
		}

		output, err := os.Create(os.Args[2] + os.Args[1])
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
		}
		defer output.Close()

		for _, val := range data {
			output.WriteString(fmt.Sprintln(val))
		}
	}
}
