#include "ethsnarks.hpp"

#include "utils.hpp"

namespace ethsnarks {

/**
* Depending on the address bit, output the correct left/right inputs
* for the merkle path authentication hash
*
* 0 = left
* 1 = right
*
* There are two variables which make up each element of the path,
* the `input` and the `pathvar`, the input is the leaf or the
* output from the last hash, and the path var is part of the merkle
* tree path.
*
* The `is_right` parameter decides if the `input` is on the left or
* right of the hash. These are decided in-circuit using the following
* method:
*
* Left:
*  (is_right * input) + ((1 - is_right) * pathvar)
*
* Right:
*  ((1 - is_right) * input) + (is_right * pathvar)
*
* Each component is split into a & b sides, then added together
* so the correct variable ends up in the right or left hand side.
*/
class merkle_path_selector : public GadgetT
{
public:
    const VariableT m_input;
    const VariableT m_pathvar;
    const VariableT m_is_right;

    VariableT m_left_a;
    VariableT m_left_b;
    VariableT m_left;

    VariableT m_right_a;
    VariableT m_right_b;
    VariableT m_right;

    merkle_path_selector(
        ProtoboardT &in_pb,
        const VariableT in_input,
        const VariableT in_pathvar,
        const VariableT in_is_right,
        const std::string &in_annotation_prefix=""
    ) :
        GadgetT(in_pb, in_annotation_prefix),
        m_input(in_input),
        m_pathvar(in_pathvar),
        m_is_right(in_is_right)
    {
        m_left_a.allocate(in_pb, FMT(this->annotation_prefix, ".left_a"));
        m_left_b.allocate(in_pb, FMT(this->annotation_prefix, ".left_b"));
        m_left.allocate(in_pb, FMT(this->annotation_prefix, ".left"));

        m_right_a.allocate(in_pb, FMT(this->annotation_prefix, ".right_a"));
        m_right_b.allocate(in_pb, FMT(this->annotation_prefix, ".right_b"));
        m_right.allocate(in_pb, FMT(this->annotation_prefix, ".right"));
    }

    void generate_r1cs_constraints()
    {
        this->pb.add_r1cs_constraint(ConstraintT(1 - m_is_right, m_input, m_left_a));
        this->pb.add_r1cs_constraint(ConstraintT(m_is_right, m_pathvar, m_left_b));
        this->pb.add_r1cs_constraint(ConstraintT(1, m_left_a + m_left_b, m_left));

        this->pb.add_r1cs_constraint(ConstraintT(m_is_right, m_input, m_right_a));
        this->pb.add_r1cs_constraint(ConstraintT(1 - m_is_right, m_pathvar, m_right_b));
        this->pb.add_r1cs_constraint(ConstraintT(1, m_right_a + m_right_b, m_right));
    }

    void generate_r1cs_witness()
    {
        this->pb.val(m_left_a) = (FieldT::one() - this->pb.val(m_is_right)) * this->pb.val(m_input);
        this->pb.val(m_left_b) = this->pb.val(m_is_right) * this->pb.val(m_pathvar);
        this->pb.val(m_left) = this->pb.val(m_left_a) + this->pb.val(m_left_b);

        this->pb.val(m_right_a) = this->pb.val(m_is_right) * this->pb.val(m_input);
        this->pb.val(m_right_b) = (FieldT::one() - this->pb.val(m_is_right)) * this->pb.val(m_pathvar);
        this->pb.val(m_right) = this->pb.val(m_right_a) + this->pb.val(m_right_b);
    }

    const VariableT& left() const {
        return m_left;
    }

    const VariableT& right() const {
        return m_right;
    }
};


const VariableArrayT merkle_tree_IVs (ProtoboardT &in_pb)
{
    auto x = make_var_array(in_pb, 29);
    std::vector<FieldT> level_IVs = {
        FieldT("149674538925118052205057075966660054952481571156186698930522557832224430770"),
        FieldT("9670701465464311903249220692483401938888498641874948577387207195814981706974"),
        FieldT("18318710344500308168304415114839554107298291987930233567781901093928276468271"),
        FieldT("6597209388525824933845812104623007130464197923269180086306970975123437805179"),
        FieldT("21720956803147356712695575768577036859892220417043839172295094119877855004262"),
        FieldT("10330261616520855230513677034606076056972336573153777401182178891807369896722"),
        FieldT("17466547730316258748333298168566143799241073466140136663575045164199607937939"),
        FieldT("18881017304615283094648494495339883533502299318365959655029893746755475886610"),
        FieldT("21580915712563378725413940003372103925756594604076607277692074507345076595494"),
        FieldT("12316305934357579015754723412431647910012873427291630993042374701002287130550"),
        FieldT("18905410889238873726515380969411495891004493295170115920825550288019118582494"),
        FieldT("12819107342879320352602391015489840916114959026915005817918724958237245903353"),
        FieldT("8245796392944118634696709403074300923517437202166861682117022548371601758802"),
        FieldT("16953062784314687781686527153155644849196472783922227794465158787843281909585"),
        FieldT("19346880451250915556764413197424554385509847473349107460608536657852472800734"),
        FieldT("14486794857958402714787584825989957493343996287314210390323617462452254101347"),
        FieldT("11127491343750635061768291849689189917973916562037173191089384809465548650641"),
        FieldT("12217916643258751952878742936579902345100885664187835381214622522318889050675"),
        FieldT("722025110834410790007814375535296040832778338853544117497481480537806506496"),
        FieldT("15115624438829798766134408951193645901537753720219896384705782209102859383951"),
        FieldT("11495230981884427516908372448237146604382590904456048258839160861769955046544"),
        FieldT("16867999085723044773810250829569850875786210932876177117428755424200948460050"),
        FieldT("1884116508014449609846749684134533293456072152192763829918284704109129550542"),
        FieldT("14643335163846663204197941112945447472862168442334003800621296569318670799451"),
        FieldT("1933387276732345916104540506251808516402995586485132246682941535467305930334"),
        FieldT("7286414555941977227951257572976885370489143210539802284740420664558593616067"),
        FieldT("16932161189449419608528042274282099409408565503929504242784173714823499212410"),
        FieldT("16562533130736679030886586765487416082772837813468081467237161865787494093536"),
        FieldT("6037428193077828806710267464232314380014232668931818917272972397574634037180")
    };
    x.fill_with_field_elements(in_pb, level_IVs);

    // Add constraints to ensure IVs remain the same
    size_t i;
    for( i = 0; i < level_IVs.size(); i++ ) {
        in_pb.add_r1cs_constraint(
            ConstraintT(1, x[i], level_IVs[i]));
    }

    return x;
}

/**
* Merkle path authenticator
*/
template<typename HashT>
class merkle_path_authenticator : public GadgetT
{
public:
    const size_t m_depth;
    const VariableArrayT m_address_bits;
    const VariableT m_leaf;
    const VariableT m_expected_root;
    const VariableArrayT m_path;

    std::vector<merkle_path_selector> m_selectors;
    std::vector<HashT> m_hashers;

    merkle_path_authenticator(
        ProtoboardT &in_pb,
        const size_t in_depth,
        const VariableArrayT in_address_bits,
        const VariableArrayT in_IVs,
        const VariableT in_leaf,
        const VariableT in_expected_root,
        const VariableArrayT in_path,
        const std::string &in_annotation_prefix = ""
    ) :
        GadgetT(in_pb, FMT(in_annotation_prefix, " merkle_path_authenticator")),
        m_depth(in_depth),
        m_address_bits(in_address_bits),
        m_leaf(in_leaf),
        m_expected_root(in_expected_root),
        m_path(in_path)
    {
        assert( in_depth > 0 );
        assert( in_address_bits.size() == in_depth );
        assert( in_IVs.size() == in_depth );

        for( size_t i = 0; i < m_depth; i++ )
        {
            if( i == 0 )
            {
                m_selectors.push_back(
                    merkle_path_selector(
                        in_pb, in_leaf, in_path[i], in_address_bits[i]));
            }
            else {
                m_selectors.push_back(
                    merkle_path_selector(
                        in_pb, m_hashers[i - 1].result(), in_path[i], in_address_bits[i]));
            }

            m_hashers.push_back(HashT(
                in_pb, in_IVs[i],
                {m_selectors[i].left(), m_selectors[i].right()},
                FMT(this->annotation_prefix, " hasher_%zu", i)));
        }
    }

    const VariableT calculated_root() const
    {
        return m_hashers[ m_hashers.size() - 1 ].result();
    }

    bool is_valid() const
    {
        return this->pb.val(calculated_root()) == this->pb.val(m_expected_root);
    }

    void generate_r1cs_constraints()
    {
        size_t i;
        for( i = 0; i < m_hashers.size(); i++ )
        {
            m_selectors[i].generate_r1cs_constraints();
            m_hashers[i].generate_r1cs_constraints();
        }

        // Ensure root matches calculated path hash
        this->pb.add_r1cs_constraint(
            ConstraintT(1, m_hashers[i-1].result(), m_expected_root));
    }

    void generate_r1cs_witness()
    {
        size_t i;
        for( i = 0; i < m_hashers.size(); i++ )
        {
            m_selectors[i].generate_r1cs_witness();
            m_hashers[i].generate_r1cs_witness();
        }
    }
};

// ethsnarks
}
