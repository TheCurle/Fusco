/***********
 * GEMWIRE *
 *  FUSCO  *
 ***********/

template <typename T>
class Visitor {
public:
    Visitor() {}
    virtual ~Visitor();
    virtual T visitBinaryExpression();
    virtual T visitGroupingExpression();
    virtual T visitUnaryExpression();
    virtual T visitLiteralExpression();
};
