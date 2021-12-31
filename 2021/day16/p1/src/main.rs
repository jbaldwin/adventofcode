use aoc_core;

fn hex_to_bin(hex: &str) -> String {
    let mut out = String::new();
    for c in hex.chars() {
        match c {
            '0' => out.push_str("0000"),
            '1' => out.push_str("0001"),
            '2' => out.push_str("0010"),
            '3' => out.push_str("0011"),
            '4' => out.push_str("0100"),
            '5' => out.push_str("0101"),
            '6' => out.push_str("0110"),
            '7' => out.push_str("0111"),
            '8' => out.push_str("1000"),
            '9' => out.push_str("1001"),
            'A' => out.push_str("1010"),
            'B' => out.push_str("1011"),
            'C' => out.push_str("1100"),
            'D' => out.push_str("1101"),
            'E' => out.push_str("1110"),
            'F' => out.push_str("1111"),
            _ => panic!("invalid hex char {}", c),
        }
    }
    out
}

#[derive(Debug)]
enum TypeId {
    Operator = 0,
    LiteralValue = 4,
}

#[derive(Debug)]
struct Packet {
    version: i16,
    type_id: TypeId,
    data: Option<i64>,
}

fn parse_packet_headers(mut bin: &str) -> (i16, TypeId) {
    let version_str = &bin[..3];
    bin = &bin[3..];
    let version = i16::from_str_radix(version_str, 2).unwrap();

    let type_id_str = &bin[..3];
    let type_id = match type_id_str {
        "100" => TypeId::LiteralValue,
        _ => TypeId::Operator,
    };

    println!("{}\n{}-{:?}", bin, version, type_id);

    (version, type_id)
}

fn parse_packet_literal_value(mut bin: &str, version: i16) -> (Packet, usize) {
    let mut used: usize = 0;
    let mut data_str = String::new();
    loop {
        let cont = &bin[..1];
        bin = &bin[1..];
        let bits = &bin[..4];
        bin = &bin[4..];
        used += 5;

        data_str.push_str(&bits);

        if cont == "0" {
            let data = i64::from_str_radix(&data_str, 2).unwrap();
            println!("data={}", data);

            return (
                Packet {
                    version,
                    type_id: TypeId::LiteralValue,
                    data: Some(data),
                },
                used,
            );
        }
    }
}

fn parse_packets_len(mut bin: &str, n: usize) -> Vec<Packet> {
    let mut packets: Vec<Packet> = Vec::new();
    let mut used_size: usize = 0;

    while used_size < n {
        let (mut sub_packets, sub_used_size) = parse_packets(bin);
        packets.append(&mut sub_packets);
        used_size += sub_used_size;

        bin = &bin[sub_used_size..];
    }

    packets
}

fn parse_packets_count(mut bin: &str, n: usize) -> (Vec<Packet>, usize) {
    let mut packets: Vec<Packet> = Vec::new();
    let mut used_count: usize = 0;
    let mut used_size: usize = 0;

    while used_count < n {
        let (mut sub_packets, sub_used_size) = parse_packets(bin);
        packets.append(&mut sub_packets);
        used_count += 1;
        used_size += sub_used_size;

        // Strip just used packet off.
        bin = &bin[sub_used_size..];
    }

    (packets, used_size)
}

fn parse_packets(mut bin: &str) -> (Vec<Packet>, usize) {
    let mut packets: Vec<Packet> = Vec::new();
    let mut used: usize = 6;

    let (version, type_id) = parse_packet_headers(bin);
    bin = &bin[6..]; // Strip headers.

    println!("{}, {:?}", version, type_id);

    match type_id {
        TypeId::LiteralValue => {
            let (packet, sub_used) = parse_packet_literal_value(bin, version);
            packets.push(packet);
            used += sub_used;
        }
        TypeId::Operator => {
            packets.push(Packet {
                version: version,
                type_id: type_id,
                data: None,
            });

            let len_type_id_str = &bin[..1];
            bin = &bin[1..];
            match len_type_id_str {
                "0" => {
                    let total_len_bits_str = &bin[..15];
                    bin = &bin[15..];
                    let total_len_bits = usize::from_str_radix(&total_len_bits_str, 2).unwrap();
                    let sub_packets_str = &bin[..total_len_bits];
                    let mut sub_packets = parse_packets_len(sub_packets_str, total_len_bits);
                    packets.append(&mut sub_packets);

                    used += 1 + 15 + total_len_bits;
                }
                "1" => {
                    let num_sub_packets_str = &bin[..11];
                    bin = &bin[11..];
                    let num_sub_packets = usize::from_str_radix(&num_sub_packets_str, 2).unwrap();
                    let (mut sub_packets, sub_used) = parse_packets_count(bin, num_sub_packets);
                    packets.append(&mut sub_packets);

                    used += 1 + 11 + sub_used;
                }
                _ => panic!("malformed operator packet"),
            }
        }
    }

    (packets, used)
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();

    let binary = hex_to_bin(&contents);

    let mut version_sum: i16 = 0;
    let (packets, _bits_used) = parse_packets(&binary);
    for packet in packets.iter() {
        println!("{:?}", packet);
        version_sum += packet.version;
    }
    println!("version_sum={}", version_sum);
}
