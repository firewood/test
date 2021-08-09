#!/usr/bin/env python3

import random

def match(rates, a, b, w, l):
    if random.random() < rates[a] / (rates[a] + rates[b]):
        w.append(a)
        l.append(b)
    else:
        w.append(b)
        l.append(a)

def qualification(rates):
    results = []
    for block in range(0, 2):
        w = []
        l = []
        for x in [(0, 1), (1, 2), (2, 0)]:
            match(rates, block * 3 + x[0], block * 3 + x[1], w, l)
        v = []
        for x in range(0, 3):
            i = block * 3 + x
            v.append((len([y for y in w if y == i]), i))
        v.sort()
        v.reverse()
        order = [x[1] for x in v]
        results.append(order)
    return results

def double_elimination(rates, results):
    w = []
    l = []
    for x in range(3):
        match(rates, results[0][x], results[1][x], w, l)
    match(rates, w[1], w[2], w, l)
    # Semifinal
    match(rates, w[0], w[3], w, l)
    # Consolation match
    match(rates, l[0], l[1], w, l)
    match(rates, w[5], l[3], w, l)
    match(rates, w[6], l[4], w, l)
    # 3rd
    match(rates, l[6], l[7], w, l)
    # 1st, 2nd
    match(rates, w[4], w[7], w, l)
    return [w[9], l[9], w[8]]

def round_robin_tournament(rates):
    w = []
    l = []
    d = [[0] * len(rates) for x in rates]
    for i in range(len(rates)):
        for j in range(i + 1, len(rates)):
            match(rates, i, j, w, l)
            d[i][j] = w[-1] == i
            d[j][i] = w[-1] == j
    counts = []
    for i in range(len(rates)):
        counts.append((len([y for y in w if y == i]), rates[i], i))
    counts.sort()
    counts.reverse()
    if counts[0][0] == counts[1][0] and d[counts[1][2]][counts[0][2]]:
        return counts[1][2]
    return counts[0][2]

counts = [0, 0]
for x in range(0, 100000):
    rates = [random.random() for x in range(6)]
#    rates = sorted([0.5 + random.random() / 2 for x in range(6)])
    order = [(rates[i], i) for i in range(len(rates))]
    order.sort()
    order.reverse()

    winner = round_robin_tournament(rates)
    if order[0][1] == winner:
        counts[0] += 1

    results = qualification(rates)
    winners = double_elimination(rates, results)
    if order[0][1] == winners[0]:
        counts[1] += 1

print(counts)
