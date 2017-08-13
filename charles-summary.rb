#!/usr/bin/env ruby

require 'csv'

h = {}
CSV.new(STDIN, headers: true, :row_sep => "\r\n").each do |row|
  unless row[0] =~ /^http.:\/\/([^\/]*)\//
    puts "ERROR"
    break
  end
  host = $1
  req_sz = row[25].to_i
  res_sz = row[26].to_i
  h[host] = (h[host] || 0) + res_sz
end
a = h.to_a.sort{ |a, b| b[1] <=> a[1] }
total = a.reduce(0) { |a, b| a + b[1] }
a << ['total', total]
a.each do |row|
#  printf "%s %d %.2fMB\n", row[0], row[1], row[1] / 1000000.0
  printf "|%s|%d|%.2fMB|\n", row[0], row[1], row[1] / 1000000.0
end
