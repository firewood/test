#!/usr/bin/env ruby

require 'openssl'
require 'base64'

def decode_pubkey(content)
  key = OpenSSL::PKey::RSA.new(content)
  key && [key.public_key.e, key.public_key.n] || nil
end

def decode_ssh_pubkey(content)
  s = content.split(' ')
  return nil unless s.size >= 2 && s[0] == 'ssh-rsa' && s[1].slice(0, 4) == 'AAAA'
  bin = Base64.decode64(s[1])
  prefix = [7].pack('N') + 'ssh-rsa'
  return nil unless bin.slice!(0, prefix.length) == prefix

  data = []
  until bin.empty?
    header = bin.slice!(0, 4)
    return nil unless header.length == 4
    bytes = header.unpack('N').first
    number = bin.slice!(0, bytes)
    return nil unless number.length == bytes
    data << OpenSSL::BN.new(number, 2)
  end
  data.size >= 2 && data || nil
end

def fingerprint(filename)
  content = File.read(filename)
  e, n = decode_ssh_pubkey(content) || decode_pubkey(content)
  return nil unless n
  data_string = [7].pack('N') + 'ssh-rsa' + e.to_s(0) + n.to_s(0)
  OpenSSL::Digest::MD5.hexdigest(data_string).scan(/../).join(':')
end

puts fingerprint(ARGV[0] || 'pubkey')
