import random, math


if __name__ == '__main__':
    numbers = []
    for _ in range(500):
        current = math.ceil(random.random() * 1000)
        numbers.append(str(current))
        numbers.append(str(-current))
    random.shuffle(numbers)

    with open('numbers.txt', mode='w') as file:
        file.write('\n'.join(numbers))
