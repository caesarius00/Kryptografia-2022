require './modular_arithmetic.rb'
include ModularArithmetic

#autor - Cezary Kwella

def generate_keys()
    #get p and g from elgamal.txt

    p=0
    g=0
    
    File.open("elgamal.txt", "r") do |f|
        p = f.readline.to_i
        g = f.readline.to_i
    end

    b = rand(1..p-1)

    beta = powmod(g, b, p) #g^b mod p

    File.open("private.txt", "w") do |f|
        f.puts p
        f.puts g
        f.puts b
    end

    File.open("public.txt", "w") do |f|
        f.puts p
        f.puts g
        f.puts beta
    end
end

def encrypt()
    p=0
    g=0
    beta=0
    #get p, g, beta from public.txt
    File.open("public.txt", "r") do |f|
        p = f.readline.to_i
        g = f.readline.to_i
        beta = f.readline.to_i
    end

    #get message from plain.txt
    message = File.read("plain.txt")

    k = rand(1..p-1)

    m = message.unpack("B*")[0].to_i(2)

    if(m>=p) 
        puts("Message is too long")
        exit(1)
    end
    
    alpha = powmod(g, k, p) #g^k mod p
    beta_k = powmod(beta, k, p) #beta^k mod p
    gamma = (m * beta_k) % p

    File.open("crypto.txt", "w") do |f|
        f.puts alpha
        f.puts gamma
    end
end

def decrypt()

    #p, g, beta, alpha, gamma from crypto.txt
    p=0
    g=0
    beta=0
    alpha=0
    gamma=0
    b=0

    File.open("crypto.txt", "r") do |f|
        alpha = f.readline.to_i
        gamma = f.readline.to_i
    end

    File.open("private.txt", "r") do |f|
        p = f.readline.to_i
        g = f.readline.to_i
        b = f.readline.to_i
    end

    beta_to_k = powmod(alpha, b, p) #alpha^b mod p
    beta_to_k_inv = invert(beta_to_k, p) #beta_to_k^-1 mod p
    m = (gamma * beta_to_k_inv) % p #gamma * beta_to_k^-1 mod p

    m_length = m.to_s(2).length
    x = m_length%8
    m_length = m_length + (8-x)

    message = m.to_s(2).rjust(m_length, '0')
    message = [message].pack("B*")

    File.open("decrypt.txt", "w") do |f|
        f.puts message
    end

end

def signature()
    #private.txt
    p=0
    g=0
    b=0

    File.open("private.txt", "r") do |f|
        p = f.readline.to_i
        g = f.readline.to_i
        b = f.readline.to_i
    end

    #get message from message.txt
    message = File.read("message.txt")

    m=message.unpack("B*")[0].to_i(2)

    if(m>=p) 
        puts("Message is too long")
        exit(1)
    end

    k=get_k(p)

    r=powmod(g, k, p) #g^k mod p

    k_inv=invert(k, p-1) #k^-1 mod p-1

    x=(m-b*r)*k_inv % (p-1)

    File.open("signature.txt", "w") do |f|
        f.puts r
        f.puts x
    end
    
end

def get_k(p)

    #k is a random number from 1 to p-1
    #k is coprime with p-1

    k=0

    loop do
        k = rand(1..p-1)
        if NWD(k, p-1) == 1
            break
        end
    end

    return k
end

def NWD(a, b)
    while b != 0
        c = a % b
        a = b
        b = c
    end
    return a
end

def verify()

    #public.txt
    p=0
    g=0
    beta=0

    File.open("public.txt", "r") do |f|
        p = f.readline.to_i
        g = f.readline.to_i
        beta = f.readline.to_i
    end

    #message.txt
    message = File.read("message.txt")

    m=message.unpack("B*")[0].to_i(2)

    if(m>=p) 
        puts("Message is too long")
        exit(1)
    end

    #signature.txt
    r=0
    x=0

    File.open("signature.txt", "r") do |f|
        r = f.readline.to_i
        x = f.readline.to_i
    end

    if r<1 || r>p-1
        puts("r is not in range")
        exit(1)
    end

    if x<1 || x>p-1
        puts("x is not in range")
        exit(1)
    end

    if powmod(g, m, p) == (powmod(beta, r, p) * powmod(r, x, p)) % p
        puts("T")
        File.open("verify.txt", "w") do |f|
            f.puts "T"
        end
    else
        puts("N")
        File.open("verify.txt", "w") do |f|
            f.puts "N"
        end
    end


end

#switch (ARGV[0]) -k -e -d -s -v
case ARGV[0]
when "-k"
    generate_keys()
when "-e"
    encrypt()
when "-d"
    decrypt()
when "-s"
    signature()
when "-v"
    verify()
else
    puts("Wrong argument")
end