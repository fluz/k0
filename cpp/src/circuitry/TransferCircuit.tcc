#ifndef K0_TRANSFERCIRCUIT_TCC
#define K0_TRANSFERCIRCUIT_TCC

#include "serialization.hpp"
#include "util.h"

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
TransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT>
k0::make_transfer_circuit(size_t tree_height) {
    protoboard<FieldT> *pb = new protoboard<FieldT>();

    pb_variable_array<FieldT> *rt_packed = new pb_variable_array<FieldT>();
    rt_packed->allocate(*pb, 2, "rt_packed");

    pb_variable_array<FieldT> *in_0_sn_packed = new pb_variable_array<FieldT>();
    in_0_sn_packed->allocate(*pb, 2, "in_0_sn_packed");

    pb_variable_array<FieldT> *in_1_sn_packed = new pb_variable_array<FieldT>();
    in_1_sn_packed->allocate(*pb, 2, "in_1_sn_packed");

    pb_variable_array<FieldT> *out_0_cm_packed = new pb_variable_array<FieldT>();
    out_0_cm_packed->allocate(*pb, 2, "out_0_cm_packed");

    pb_variable_array<FieldT> *out_1_cm_packed = new pb_variable_array<FieldT>();
    out_1_cm_packed->allocate(*pb, 2, "out_1_cm_packed");

    pb_variable<FieldT> *callee_public = new pb_variable<FieldT>();
    callee_public->allocate(*pb, "callee_public");

    pb->set_input_sizes(11);

    pb_variable<FieldT> *ZERO = new pb_variable<FieldT>();
    ZERO->allocate(*pb, "ZERO");

    digest_variable<FieldT> *rt_bits =
            new digest_variable<FieldT>(*pb, 256, "rt_bits");

    pb_variable_array<FieldT> *in_0_v_bits = new pb_variable_array<FieldT>();
    in_0_v_bits->allocate(*pb, 64, "in_0_v_bits");

    pb_variable_array<FieldT> *in_0_a_sk_bits = new pb_variable_array<FieldT>();
    in_0_a_sk_bits->allocate(*pb, 256, "in_0_a_sk_bits");

    pb_variable_array<FieldT> *in_0_rho_bits = new pb_variable_array<FieldT>();
    in_0_rho_bits->allocate(*pb, 256, "in_0_rho_bits");

    pb_variable_array<FieldT> *in_0_r_bits = new pb_variable_array<FieldT>();
    in_0_r_bits->allocate(*pb, 384, "in_0_r_bits");

    pb_variable_array<FieldT> *in_0_address_bits = new pb_variable_array<FieldT>();
    in_0_address_bits->allocate(*pb, tree_height, "address_bits");

    merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *in_0_path =
            new merkle_authentication_path_variable<FieldT, MerkleTreeHashT>(
                    *pb, tree_height, "in_0_path");

    auto in_0_a_pk_bits = make_shared<digest_variable<FieldT>>(*pb, 256,
                                                               "a_pk_bits");

    digest_variable<FieldT> *in_0_cm_bits =
            new digest_variable<FieldT>(*pb, 256, "in_0_cm_bits");

    auto in_0_sn_bits = make_shared<digest_variable<FieldT>>(*pb, 256,
                                                             "in_0_sn_bits");


    pb_variable_array<FieldT> *in_1_v_bits = new pb_variable_array<FieldT>();
    in_1_v_bits->allocate(*pb, 64, "in_1_v_bits");

    pb_variable_array<FieldT> *in_1_a_sk_bits = new pb_variable_array<FieldT>();
    in_1_a_sk_bits->allocate(*pb, 256, "in_1_a_sk_bits");

    pb_variable_array<FieldT> *in_1_rho_bits = new pb_variable_array<FieldT>();
    in_1_rho_bits->allocate(*pb, 256, "in_1_rho_bits");

    pb_variable_array<FieldT> *in_1_r_bits = new pb_variable_array<FieldT>();
    in_1_r_bits->allocate(*pb, 384, "in_1_r_bits");

    pb_variable_array<FieldT> *in_1_address_bits = new pb_variable_array<FieldT>();
    in_1_address_bits->allocate(*pb, tree_height, "address_bits");

    merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *in_1_path =
            new merkle_authentication_path_variable<FieldT, MerkleTreeHashT>(
                    *pb, tree_height, "in_1_path");

    auto in_1_a_pk_bits = make_shared<digest_variable<FieldT>>(*pb, 256,
                                                               "a_pk_bits");

    digest_variable<FieldT> *in_1_cm_bits =
            new digest_variable<FieldT>(*pb, 256, "in_1_cm_bits");

    auto in_1_sn_bits = make_shared<digest_variable<FieldT>>(*pb, 256,
                                                             "in_1_sn_bits");

    pb_variable_array<FieldT> *total_value_bits = new pb_variable_array<FieldT>();
    total_value_bits->allocate(*pb, 64, "total_value_bits");

    pb_variable_array<FieldT> *out_0_v_bits = new pb_variable_array<FieldT>();
    out_0_v_bits->allocate(*pb, 64, "out_0_v_bits");

    pb_variable_array<FieldT> *out_0_a_pk_bits = new pb_variable_array<FieldT>();
    out_0_a_pk_bits->allocate(*pb, 256, "out_0_a_pk_bits");

    pb_variable_array<FieldT> *out_0_rho_bits = new pb_variable_array<FieldT>();
    out_0_rho_bits->allocate(*pb, 256, "out_0_rho_bits");

    pb_variable_array<FieldT> *out_0_r_bits = new pb_variable_array<FieldT>();
    out_0_r_bits->allocate(*pb, 384, "out_0_r_bits");

    digest_variable<FieldT> *out_0_cm_bits =
            new digest_variable<FieldT>(*pb, 256, "out_0_cm_bits");

    pb_variable_array<FieldT> *out_1_v_bits = new pb_variable_array<FieldT>();
    out_1_v_bits->allocate(*pb, 64, "out_1_v_bits");

    pb_variable_array<FieldT> *out_1_a_pk_bits = new pb_variable_array<FieldT>();
    out_1_a_pk_bits->allocate(*pb, 256, "out_1_a_pk_bits");

    pb_variable_array<FieldT> *out_1_rho_bits = new pb_variable_array<FieldT>();
    out_1_rho_bits->allocate(*pb, 256, "out_1_rho_bits");

    pb_variable_array<FieldT> *out_1_r_bits = new pb_variable_array<FieldT>();
    out_1_r_bits->allocate(*pb, 384, "out_1_r_bits");

    digest_variable<FieldT> *out_1_cm_bits =
            new digest_variable<FieldT>(*pb, 256, "out_1_cm_bits");

    pb_variable<FieldT> *callee_private = new pb_variable<FieldT>();
    callee_private->allocate(*pb, "callee_private");

    multipacking_gadget<FieldT> *rt_packer =
            new multipacking_gadget<FieldT>(
                    *pb, rt_bits->bits, *rt_packed, 128, "rt_packer");

    multipacking_gadget<FieldT> *in_0_sn_packer =
            new multipacking_gadget<FieldT>(
                    *pb, in_0_sn_bits->bits, *in_0_sn_packed, 128,
                    "in_0_sn_packer");

    multipacking_gadget<FieldT> *in_1_sn_packer =
            new multipacking_gadget<FieldT>(
                    *pb, in_1_sn_bits->bits, *in_1_sn_packed, 128,
                    "in_1_sn_packer");

    multipacking_gadget<FieldT> *out_0_cm_packer =
            new multipacking_gadget<FieldT>(
                    *pb, out_0_cm_bits->bits, *out_0_cm_packed, 128,
                    "out_0_cm_packer");

    multipacking_gadget<FieldT> *out_1_cm_packer =
            new multipacking_gadget<FieldT>(
                    *pb, out_1_cm_bits->bits, *out_1_cm_packed, 128,
                    "out_0_cm_packer");

    auto in_0_note_gadget =
            new input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT>(
                    tree_height,
                    *pb,
                    *ZERO,
                    *rt_bits,
                    *in_0_v_bits,
                    *in_0_a_sk_bits,
                    *in_0_rho_bits,
                    *in_0_r_bits,
                    *in_0_address_bits,
                    *in_0_path,
                    in_0_a_pk_bits,
                    *in_0_cm_bits,
                    in_0_sn_bits,
                    "in_0_note_gadget");

    auto in_1_note_gadget =
            new input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT>(
                    tree_height,
                    *pb,
                    *ZERO,
                    *rt_bits,
                    *in_1_v_bits,
                    *in_1_a_sk_bits,
                    *in_1_rho_bits,
                    *in_1_r_bits,
                    *in_1_address_bits,
                    *in_1_path,
                    in_1_a_pk_bits,
                    *in_1_cm_bits,
                    in_1_sn_bits,
                    "in_1_note_gadget");

    auto out_0_cm_gadget = new cm_full_gadget<FieldT, CommitmentHashT>(
            *pb,
            *ZERO,
            *out_0_a_pk_bits,
            *out_0_rho_bits,
            *out_0_r_bits,
            *out_0_v_bits,
            *out_0_cm_bits,
            "out_0_cm_gadget");

    auto out_1_cm_gadget = new cm_full_gadget<FieldT, CommitmentHashT>(
            *pb,
            *ZERO,
            *out_1_a_pk_bits,
            *out_1_rho_bits,
            *out_1_r_bits,
            *out_1_v_bits,
            *out_1_cm_bits,
            "out_1_cm_gadget");

    pb->add_r1cs_constraint(
            r1cs_constraint<FieldT>(*ZERO, ONE, FieldT::zero()),
            "ZERO must equal zero");

    in_0_path->generate_r1cs_constraints();
    in_0_a_pk_bits->generate_r1cs_constraints();
    in_0_cm_bits->generate_r1cs_constraints();
    in_1_path->generate_r1cs_constraints();
    in_1_a_pk_bits->generate_r1cs_constraints();
    in_1_cm_bits->generate_r1cs_constraints();
    out_0_cm_bits->generate_r1cs_constraints();
    out_1_cm_bits->generate_r1cs_constraints();

    rt_packer->generate_r1cs_constraints(true);
    in_0_sn_packer->generate_r1cs_constraints(true);
    in_0_note_gadget->generate_r1cs_constraints();
    in_1_sn_packer->generate_r1cs_constraints(true);
    in_1_note_gadget->generate_r1cs_constraints();

    // sum of input values must equal sum of output values
    auto sum_input_values = pb_packing_sum<FieldT>(*in_0_v_bits) +
                            pb_packing_sum<FieldT>(*in_1_v_bits);
    auto sum_output_values = pb_packing_sum<FieldT>(*out_0_v_bits) +
                             pb_packing_sum<FieldT>(*out_1_v_bits);
    pb->add_r1cs_constraint(
            r1cs_constraint<FieldT>(1, sum_input_values, sum_output_values),
            "sum of input values must equal sum of output values");

    // sum of output values is equal to total value
    pb->add_r1cs_constraint(
            r1cs_constraint<FieldT>(1,
                                    sum_input_values,
                                    pb_packing_sum<FieldT>(*total_value_bits)),
            "sum of output values must equal total value");

    // total value is a 64 bit number
    for (size_t i = 0; i < 64; i++) {
        auto bits = *total_value_bits;
        generate_boolean_r1cs_constraint<FieldT>(
                *pb,
                bits[i],
                "total_value_bits bit " + to_string(i) + "is boolean");
    }

    pb->add_r1cs_constraint(
            r1cs_constraint<FieldT>(*callee_public, ONE, *callee_private),
            "callee_public must equal callee_private");

    out_0_cm_gadget->generate_r1cs_constraints();
    out_0_cm_packer->generate_r1cs_constraints(true);
    out_1_cm_gadget->generate_r1cs_constraints();
    out_1_cm_packer->generate_r1cs_constraints(true);

    TransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT> circuit{
            pb,
            rt_packed,
            in_0_sn_packed,
            in_1_sn_packed,
            out_0_cm_packed,
            out_1_cm_packed,
            callee_public,

            ZERO,
            rt_bits,

            in_0_v_bits,
            in_0_a_sk_bits,
            in_0_rho_bits,
            in_0_r_bits,
            in_0_address_bits,
            in_0_path,
            in_0_a_pk_bits,
            in_0_cm_bits,
            in_0_sn_bits,

            in_1_v_bits,
            in_1_a_sk_bits,
            in_1_rho_bits,
            in_1_r_bits,
            in_1_address_bits,
            in_1_path,
            in_1_a_pk_bits,
            in_1_cm_bits,
            in_1_sn_bits,

            total_value_bits,

            out_0_v_bits,
            out_0_a_pk_bits,
            out_0_rho_bits,
            out_0_r_bits,
            out_0_cm_bits,

            out_1_v_bits,
            out_1_a_pk_bits,
            out_1_rho_bits,
            out_1_r_bits,
            out_1_cm_bits,

            callee_private,

            rt_packer,
            in_0_sn_packer,
            in_1_sn_packer,
            out_0_cm_packer,
            out_1_cm_packer,
            in_0_note_gadget,
            in_1_note_gadget,
            out_0_cm_gadget,
            out_1_cm_gadget
    };

    return circuit;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void
k0::print(TransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT> &c) {
    cout << endl;
    cout << endl;
    cout
            << "************************ TRANSFER CIRCUIT *************************"
            << endl;
    cout << endl;
    cout << "ROOT " << bits2hex(c.rt_bits->get_digest()) << endl;
    cout << endl;
    cout << "IN 0" << endl;
    cout << "v_bits       " << bits2hex(c.in_0_v_bits->get_bits(*c.pb)) << endl;
    cout << "v val        " << c.in_0_v_bits->get_field_element_from_bits(*c.pb)
         << endl;
    cout << "a_sk_bits    " << bits2hex(c.in_0_a_sk_bits->get_bits(*c.pb))
         << endl;
    cout << "rho_bits     " << bits2hex(c.in_0_rho_bits->get_bits(*c.pb))
         << endl;
    cout << "r_bits       " << bits2hex(c.in_0_r_bits->get_bits(*c.pb)) << endl;
    cout << "address_bits " << bits2hex(c.in_0_address_bits->get_bits(*c.pb))
         << endl;
    FieldT in_0_address = c.in_0_address_bits->get_field_element_from_bits(
            *c.pb);
    cout << "address val  " << in_0_address << endl;
    cout << "a_pk_bits    " << bits2hex(c.in_0_a_pk_bits->get_digest()) << endl;
    cout << "cm_bits      " << bits2hex(c.in_0_cm_bits->get_digest()) << endl;
    cout << "path" << endl;
    for (auto bv : c.in_0_path->get_authentication_path(
            in_0_address.as_bigint().as_ulong())) {
        cout << bits2hex(bv) << endl;
    }

    cout << endl;
    cout << "IN 1" << endl;
    cout << "v_bits       " << bits2hex(c.in_1_v_bits->get_bits(*c.pb)) << endl;
    cout << "v val        " << c.in_1_v_bits->get_field_element_from_bits(*c.pb)
         << endl;
    cout << "a_sk_bits    " << bits2hex(c.in_1_a_sk_bits->get_bits(*c.pb))
         << endl;
    cout << "rho_bits     " << bits2hex(c.in_1_rho_bits->get_bits(*c.pb))
         << endl;
    cout << "r_bits       " << bits2hex(c.in_1_r_bits->get_bits(*c.pb)) << endl;
    cout << "address_bits " << bits2hex(c.in_1_address_bits->get_bits(*c.pb))
         << endl;
    FieldT in_1_address = c.in_1_address_bits->get_field_element_from_bits(
            *c.pb);
    cout << "address val  " << in_1_address << endl;
    cout << "a_pk_bits    " << bits2hex(c.in_1_a_pk_bits->get_digest()) << endl;
    cout << "cm_bits      " << bits2hex(c.in_1_cm_bits->get_digest()) << endl;
    cout << "path" << endl;
    for (auto bv : c.in_1_path->get_authentication_path(
            in_1_address.as_bigint().as_ulong())) {
        cout << bits2hex(bv) << endl;
    }

    cout << endl;
    cout << "OUT 0" << endl;
    cout << "v_bits       " << bits2hex(c.out_0_v_bits->get_bits(*c.pb))
         << endl;
    cout << "v val        "
         << c.out_0_v_bits->get_field_element_from_bits(*c.pb) << endl;
    cout << "a_pk_bits    " << bits2hex(c.out_0_a_pk_bits->get_bits(*c.pb))
         << endl;
    cout << "rho_bits     " << bits2hex(c.out_0_rho_bits->get_bits(*c.pb))
         << endl;
    cout << "r_bits       " << bits2hex(c.out_0_r_bits->get_bits(*c.pb))
         << endl;
    cout << "cm_bits      " << bits2hex(c.out_0_cm_bits->get_digest()) << endl;

    cout << endl;

    cout << "OUT 1" << endl;
    cout << "v_bits       " << bits2hex(c.out_1_v_bits->get_bits(*c.pb))
         << endl;
    cout << "v val        "
         << c.out_1_v_bits->get_field_element_from_bits(*c.pb) << endl;
    cout << "a_pk_bits    " << bits2hex(c.out_1_a_pk_bits->get_bits(*c.pb))
         << endl;
    cout << "rho_bits     " << bits2hex(c.out_1_rho_bits->get_bits(*c.pb))
         << endl;
    cout << "r_bits       " << bits2hex(c.out_1_r_bits->get_bits(*c.pb))
         << endl;
    cout << "cm_bits      " << bits2hex(c.out_1_cm_bits->get_digest()) << endl;

    cout << endl;
    cout
            << "*******************************************************************"
            << endl;
    cout << endl;
    cout << endl;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void k0::populate(
        TransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT> &c,
        size_t tree_height,
        bit_vector &merkle_tree_root,
        input_note &in_0,
        input_note &in_1,
        output_note &out_0,
        output_note &out_1,
        FieldT &callee) {
    c.rt_bits->generate_r1cs_witness(merkle_tree_root);

    auto in_0_address_bits = int_to_bits<FieldT>(in_0.address, tree_height);
    auto in_0_v_bits = uint64_to_bits(in_0.v);
    c.in_0_a_sk_bits->fill_with_bits(*c.pb, in_0.a_sk);
    c.in_0_rho_bits->fill_with_bits(*c.pb, in_0.rho);
    c.in_0_r_bits->fill_with_bits(*c.pb, in_0.r);
    c.in_0_address_bits->fill_with_bits(*c.pb, in_0_address_bits);
    c.in_0_v_bits->fill_with_bits(*c.pb, in_0_v_bits);
    c.in_0_path->generate_r1cs_witness(in_0.address, in_0.path);

    auto in_1_address_bits = int_to_bits<FieldT>(in_1.address, tree_height);
    auto in_1_v_bits = uint64_to_bits(in_1.v);
    c.in_1_a_sk_bits->fill_with_bits(*c.pb, in_1.a_sk);
    c.in_1_rho_bits->fill_with_bits(*c.pb, in_1.rho);
    c.in_1_r_bits->fill_with_bits(*c.pb, in_1.r);
    c.in_1_address_bits->fill_with_bits(*c.pb, in_1_address_bits);
    c.in_1_v_bits->fill_with_bits(*c.pb, in_1_v_bits);
    c.in_1_path->generate_r1cs_witness(in_1.address, in_1.path);

    auto out_0_v_bits = uint64_to_bits(out_0.v);
    c.out_0_a_pk_bits->fill_with_bits(*c.pb, out_0.a_pk);
    c.out_0_rho_bits->fill_with_bits(*c.pb, out_0.rho);
    c.out_0_r_bits->fill_with_bits(*c.pb, out_0.r);
    c.out_0_v_bits->fill_with_bits(*c.pb, out_0_v_bits);

    auto out_1_v_bits = uint64_to_bits(out_1.v);
    c.out_1_a_pk_bits->fill_with_bits(*c.pb, out_1.a_pk);
    c.out_1_rho_bits->fill_with_bits(*c.pb, out_1.rho);
    c.out_1_r_bits->fill_with_bits(*c.pb, out_1.r);
    c.out_1_v_bits->fill_with_bits(*c.pb, out_1_v_bits);

    auto total_out_v_bits = uint64_to_bits(out_0.v + out_1.v);
    c.total_value->fill_with_bits(*c.pb, total_out_v_bits);

    c.pb->val(*c.callee_public) = callee;
    c.pb->val(*c.callee_private) = callee;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void k0::generate_witness(
        TransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT> &c) {
    c.rt_packer->generate_r1cs_witness_from_bits();
    c.in_0_note_gadget->generate_r1cs_witness();
    c.in_1_note_gadget->generate_r1cs_witness();
    c.in_0_sn_packer->generate_r1cs_witness_from_bits();
    c.in_1_sn_packer->generate_r1cs_witness_from_bits();
    c.out_0_cm_gadget->generate_r1cs_witness();
    c.out_1_cm_gadget->generate_r1cs_witness();
    c.out_0_cm_packer->generate_r1cs_witness_from_bits();
    c.out_1_cm_packer->generate_r1cs_witness_from_bits();
}

#endif //K0_TRANSFERCIRCUIT_TCC
