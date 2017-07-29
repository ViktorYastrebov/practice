package builder_example;

public class main {

    public static void main(String[] args) {
      ShapeBuilder builder = new ShapeBuilder();
      Shape shape = builder.getById(Shape.ShapeType.CICLE);
      shape.print_name();
      
      BaseVisitor  printer = new PrintVisitor();
      shape.Accept(printer);
      
      BaseObserver observer = new ColoredObserver();
      shape.Attach(observer);
      shape.setColor(Shape.Color.WHITE);
      shape.Detach(observer);
    }

}
