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
    OperatorSum = 0,
    OperatorProduct = 1,
    OperatorMin = 2,
    OperatorMax = 3,
    LiteralValue = 4,
    OperatorGreaterThan = 5,
    OperatorLessThan = 6,
    OperatorEqualTo = 7,
}

#[derive(Debug)]
enum DataType {
    Literal(i64),
    Packets(Vec<Packet>),
}

#[derive(Debug)]
struct Packet {
    #[allow(dead_code)]
    version: i16,
    type_id: TypeId,
    data: DataType,
}

fn parse_packet_headers(mut bin: &str) -> (i16, TypeId) {
    let version_str = &bin[..3];
    bin = &bin[3..];
    let version = i16::from_str_radix(version_str, 2).unwrap();

    let type_id_str = &bin[..3];
    let type_id = match type_id_str {
        "000" => TypeId::OperatorSum,
        "001" => TypeId::OperatorProduct,
        "010" => TypeId::OperatorMin,
        "011" => TypeId::OperatorMax,
        "100" => TypeId::LiteralValue,
        "101" => TypeId::OperatorGreaterThan,
        "110" => TypeId::OperatorLessThan,
        "111" => TypeId::OperatorEqualTo,
        _ => panic!("Unknown type_id {}", type_id_str),
    };

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

            return (
                Packet {
                    version,
                    type_id: TypeId::LiteralValue,
                    data: DataType::Literal(data),
                },
                used,
            );
        }
    }
}

fn parse_packets_len(mut bin: &str, n: usize, version: i16, type_id: TypeId) -> Packet {
    let mut packets: Vec<Packet> = Vec::new();
    let mut used_size: usize = 0;

    while used_size < n {
        let (sub_packet, sub_used_size) = parse_packets(bin);
        packets.push(sub_packet);
        used_size += sub_used_size;

        bin = &bin[sub_used_size..];
    }

    Packet {
        version,
        type_id,
        data: DataType::Packets(packets),
    }
}

fn parse_packets_count(mut bin: &str, n: usize, version: i16, type_id: TypeId) -> (Packet, usize) {
    let mut packets: Vec<Packet> = Vec::new();
    let mut used_count: usize = 0;
    let mut used_size: usize = 0;

    while used_count < n {
        let (sub_packet, sub_used_size) = parse_packets(bin);
        packets.push(sub_packet);
        used_count += 1;
        used_size += sub_used_size;

        // Strip just used packet off.
        bin = &bin[sub_used_size..];
    }

    return (
        Packet {
            version,
            type_id,
            data: DataType::Packets(packets),
        },
        used_size,
    );
}

fn parse_packets(mut bin: &str) -> (Packet, usize) {
    let mut used: usize = 6;

    let (version, type_id) = parse_packet_headers(bin);
    bin = &bin[6..]; // Strip headers.

    match type_id {
        TypeId::LiteralValue => {
            let (packet, sub_used) = parse_packet_literal_value(bin, version);
            used += sub_used;

            return (packet, used);
        }
        // TypeId::Operator*
        _ => {
            let len_type_id_str = &bin[..1];
            bin = &bin[1..];
            match len_type_id_str {
                "0" => {
                    let total_len_bits_str = &bin[..15];
                    bin = &bin[15..];
                    let total_len_bits = usize::from_str_radix(&total_len_bits_str, 2).unwrap();
                    let sub_packets_str = &bin[..total_len_bits];
                    let packet =
                        parse_packets_len(sub_packets_str, total_len_bits, version, type_id);

                    used += 1 + 15 + total_len_bits;

                    return (packet, used);
                }
                "1" => {
                    let num_sub_packets_str = &bin[..11];
                    bin = &bin[11..];
                    let num_sub_packets = usize::from_str_radix(&num_sub_packets_str, 2).unwrap();
                    let (packet, sub_used) =
                        parse_packets_count(bin, num_sub_packets, version, type_id);

                    used += 1 + 11 + sub_used;

                    return (packet, used);
                }
                _ => panic!("malformed operator packet"),
            }
        }
    }
}

fn eval(packet: &Packet) -> i64 {
    match packet.type_id {
        TypeId::LiteralValue => match packet.data {
            DataType::Literal(value) => value,
            _ => panic!("LiteralValue has incorrect data type {:?}", packet),
        },
        TypeId::OperatorSum => match &packet.data {
            DataType::Packets(packets) => {
                let mut sum: i64 = 0;
                for p in packets.iter() {
                    sum += eval(p);
                }
                sum
            }
            _ => panic!("OperatorSum has incorrect data type {:?}", packet),
        },
        TypeId::OperatorProduct => match &packet.data {
            DataType::Packets(packets) => {
                let mut product: i64 = 1;
                for p in packets.iter() {
                    product *= eval(p);
                }
                product
            }
            _ => panic!("OperatorSum has incorrect data type {:?}", packet),
        },
        TypeId::OperatorMin => match &packet.data {
            DataType::Packets(packets) => {
                let mut min: i64 = i64::MAX;
                for p in packets.iter() {
                    min = std::cmp::min(min, eval(p));
                }
                min
            }
            _ => panic!("OperatorSum has incorrect data type {:?}", packet),
        },
        TypeId::OperatorMax => match &packet.data {
            DataType::Packets(packets) => {
                let mut max: i64 = i64::MIN;
                for p in packets.iter() {
                    max = std::cmp::max(max, eval(p));
                }
                max
            }
            _ => panic!("OperatorSum has incorrect data type {:?}", packet),
        },
        TypeId::OperatorGreaterThan => match &packet.data {
            DataType::Packets(packets) => {
                let first = eval(&packets[0]);
                let second = eval(&packets[1]);

                if first > second {
                    return 1;
                } else {
                    return 0;
                }
            }
            _ => panic!("OperatorSum has incorrect data type {:?}", packet),
        },
        TypeId::OperatorLessThan => match &packet.data {
            DataType::Packets(packets) => {
                let first = eval(&packets[0]);
                let second = eval(&packets[1]);

                if first < second {
                    return 1;
                } else {
                    return 0;
                }
            }
            _ => panic!("OperatorSum has incorrect data type {:?}", packet),
        },
        TypeId::OperatorEqualTo => match &packet.data {
            DataType::Packets(packets) => {
                let first = eval(&packets[0]);
                let second = eval(&packets[1]);

                if first == second {
                    return 1;
                } else {
                    return 0;
                }
            }
            _ => panic!("OperatorSum has incorrect data type {:?}", packet),
        },
    }
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();

    let binary = hex_to_bin(&contents);

    let (packet, _bits_used) = parse_packets(&binary);
    println!("{}", eval(&packet));
}
