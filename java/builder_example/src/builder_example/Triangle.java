package builder_example;

public class Triangle extends ColoredShape {

    private int length_s1, length_s2, length_s3;
    public Triangle() {
        super();
        length_s1 = length_s2 = length_s3 = 1;
    }

    public Triangle(int l1, int l2, int l3) {
        super();
        length_s1 = l1;
        length_s2 = l2;
        length_s3 = l3;
    }

    public void print_name() {
      System.out.println("I'm triangle" + "l1 = " + length_s1 + ", l2 =" + length_s2 + ", l3 = " + length_s3);
    }

    public ShapeType getID() {
      return Shape.ShapeType.TRIANGLE;
    }

    public void Accept(BaseVisitor  v) {
        v.Visit(this);
    }
    
    public int getLength1() {
        return length_s1;
    }
    public int getLength2() {
        return length_s2;
    }
    public int getLength3() {
       return length_s3;
    }
    
}
