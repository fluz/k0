#ifndef K0_CIRCUITRY_WITHDRAWAL_CIRCUIT_TCC
#define K0_CIRCUITRY_WITHDRAWAL_CIRCUIT_TCC

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
WithdrawalCircuit<FieldT, CommitmentHashT, MerkleTreeHashT>
make_withdrawal_circuit(size_t tree_height) {
    protoboard<FieldT> *pb = new protoboard<FieldT>();

    pb_variable_array<FieldT> *rt_packed = new pb_variable_array<FieldT>();
    rt_packed->allocate(*pb, 2, "rt_packed");

    pb_variable<FieldT> *v_packed = new pb_variable<FieldT>();
    v_packed->allocate(*pb, "v_packed");

    pb_variable_array<FieldT> *sn_packed = new pb_variable_array<FieldT>();
    sn_packed->allocate(*pb, 2, "sn_packed");

    pb_variable<FieldT> *recipient_public = new pb_variable<FieldT>();
    recipient_public->allocate(*pb, "recipient_public");

    pb->set_input_sizes(6);

    pb_variable<FieldT> *ZERO = new pb_variable<FieldT>();
    ZERO->allocate(*pb, "ZERO");

    digest_variable<FieldT> *rt_bits =
            new digest_variable<FieldT>(*pb, 256, "rt_bits");

    pb_variable_array<FieldT> *v_bits = new pb_variable_array<FieldT>();
    v_bits->allocate(*pb, 64, "v_bits");

    pb_variable_array<FieldT> *a_sk_bits = new pb_variable_array<FieldT>();
    a_sk_bits->allocate(*pb, 256, "a_sk_bits");

    pb_variable_array<FieldT> *rho_bits = new pb_variable_array<FieldT>();
    rho_bits->allocate(*pb, 256, "rho_bits");

    pb_variable_array<FieldT> *r_bits = new pb_variable_array<FieldT>();
    r_bits->allocate(*pb, 384, "r_bits");

    pb_variable_array<FieldT> *address_bits = new pb_variable_array<FieldT>();
    address_bits->allocate(*pb, tree_height, "address_bits");

    merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *path =
            new merkle_authentication_path_variable<FieldT, MerkleTreeHashT>(
                    *pb, tree_height, "merkle_authentication_path");

    pb_variable<FieldT> *recipient_private = new pb_variable<FieldT>();
    recipient_private->allocate(*pb, "recipient_private");

    auto a_pk_bits = make_shared<digest_variable<FieldT>>(*pb, 256,
                                                          "a_pk_bits");

    digest_variable<FieldT> *commitment_bits =
            new digest_variable<FieldT>(*pb, 256, "commitment_bits");

    auto sn_bits = make_shared<digest_variable<FieldT>>(*pb, 256, "sn_bits");

    multipacking_gadget<FieldT> *rt_packer =
            new multipacking_gadget<FieldT>(
                    *pb, rt_bits->bits, *rt_packed, 128, "rt_packer");

    packing_gadget<FieldT> *v_packer =
            new packing_gadget<FieldT>(*pb, *v_bits, *v_packed, "v_packer");

    multipacking_gadget<FieldT> *sn_packer =
            new multipacking_gadget<FieldT>(
                    *pb, sn_bits->bits, *sn_packed, 128, "sn_packer");

    auto note_gadget =
            new input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT>(
                    tree_height,
                    *pb,
                    *ZERO,
                    *rt_bits,
                    *v_bits,
                    *a_sk_bits,
                    *rho_bits,
                    *r_bits,
                    *address_bits,
                    *path,
                    a_pk_bits,
                    *commitment_bits,
                    sn_bits,
                    "input_note_gadget");

    pb->add_r1cs_constraint(
            r1cs_constraint<FieldT>(*ZERO, ONE, FieldT::zero()),
            "ZERO must equal zero");

    pb->add_r1cs_constraint(
            r1cs_constraint<FieldT>(*recipient_public, ONE, *recipient_private),
            "recipient_public must equal recipient_private");

    path->generate_r1cs_constraints();
    a_pk_bits->generate_r1cs_constraints();
    commitment_bits->generate_r1cs_constraints();
    sn_bits->generate_r1cs_constraints();

    rt_packer->generate_r1cs_constraints(true);
    v_packer->generate_r1cs_constraints(true);
    sn_packer->generate_r1cs_constraints(true);
    note_gadget->generate_r1cs_constraints();

    WithdrawalCircuit<FieldT, CommitmentHashT, MerkleTreeHashT> circuit{
            pb,
            rt_packed,
            v_packed,
            sn_packed,
            recipient_public,
            ZERO,
            rt_bits,
            v_bits,
            a_sk_bits,
            rho_bits,
            r_bits,
            address_bits,
            path,
            recipient_private,
            a_pk_bits,
            commitment_bits,
            sn_bits,
            rt_packer,
            v_packer,
            sn_packer,
            note_gadget
    };

    return circuit;
}

#endif //K0_CIRCUITRY_WITHDRAWAL_CIRCUIT_TCC
