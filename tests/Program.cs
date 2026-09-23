using karel_cpp;
using karelui;

internal static class Program
{
    private static int checks;
    private static void Check(bool condition, string message)
    {
        if (!condition) throw new Exception(message);
        checks++;
    }
    private static void Throws<T>(Action action) where T : Exception
    {
        try { action(); }
        catch (T) { checks++; return; }
        throw new Exception($"Expected {typeof(T).Name}");
    }

    [STAThread]
    private static void Main(string[] args)
    {
        if (args.Contains("--demo"))
        {
            TestDemo();
            return;
        }
        ApplicationConfiguration.Initialize();
        Throws<ArgumentOutOfRangeException>(() => new Karel(0, 3));
        Throws<ArgumentOutOfRangeException>(() => new Karel(3, -1));
        Throws<ArgumentOutOfRangeException>(() => new Karel(int.MaxValue, 2));
        Throws<ArgumentException>(() => new Orientation(0));
        Check(default(Orientation).AsPoint() == new Point(0, -1), "Default direction must be North");
        Orientation[] directions = [Orientation.North(), Orientation.East(), Orientation.South(), Orientation.West()];
        Point[] vectors = [new(0, -1), new(1, 0), new(0, 1), new(-1, 0)];
        for (int i = 0; i < directions.Length; i++)
        {
            Check(directions[i].AsPoint() == vectors[i], "Direction vector");
            Check(directions[i].RotateLeft().AsPoint() == vectors[(i + 3) % 4], "Left turn");
            Check(directions[i].RotateRight().AsPoint() == vectors[(i + 1) % 4], "Right turn");
            for (int amount = sbyte.MinValue; amount <= sbyte.MaxValue; amount++)
                Check(directions[i].Rotate((sbyte)amount).AsPoint() == vectors[((i + amount) % 4 + 4) % 4], "Rotation wrapping");
        }

        var karel = new Karel(3, 2);
        Check(karel.GetGridSize() == new Size(3, 2), "Grid dimensions");
        Check(karel.GetPosition() == Point.Empty, "Initial position");
        Check(karel.GetOrientation().AsPoint() == new Point(1, 0), "Initial direction");
        for (int y = 0; y < 2; y++)
            for (int x = 0; x < 3; x++)
                Check(karel.GetColorAt(x, y).ToArgb() == Color.White.ToArgb(), "Initial color");
        karel.Paint(Color.Red);
        karel.Move(2);
        karel.Paint(Color.Blue);
        Check(karel.GetColorAt(0, 0).ToArgb() == Color.Red.ToArgb(), "Reads requested cell");
        Check(karel.GetColorBeneath().ToArgb() == Color.Blue.ToArgb(), "Reads current cell");
        foreach (Point invalid in new Point[] { new(-1, 0), new(3, 0), new(0, -1), new(0, 2), new(int.MaxValue, int.MaxValue) })
            Throws<ArgumentOutOfRangeException>(() => karel.GetColorAt(invalid.X, invalid.Y));
        Throws<InvalidOperationException>(() => karel.Move());
        Throws<InvalidOperationException>(() => karel.Move(int.MaxValue));
        Throws<InvalidOperationException>(() => karel.Move(int.MinValue));
        Check(karel.GetPosition() == new Point(2, 0), "Rejected movement preserves position");
        karel.RotateRight();
        karel.Move();
        Check(karel.GetPosition() == new Point(2, 1), "Right turn updates Karel");
        karel.RotateRight();
        karel.Move(2);
        karel.RotateRight();
        karel.Move();
        Check(karel.GetPosition() == Point.Empty, "Moves in all four directions");
        karel.RotateLeft();
        Check(karel.GetOrientation().AsPoint() == new Point(-1, 0), "Left turn updates Karel");
        karel.Rotate(2);
        karel.Move();
        karel.Move(-1);
        Check(karel.GetPosition() == Point.Empty, "Signed movement");
        int changes = 0;
        karel.Changed += (_, _) => changes++;
        karel.Paint(Color.Green);
        karel.Move();
        karel.RotateRight();
        Check(changes == 3, "Model notifies UI of changes");

        using var form = new KarelUI(new Size(3, 2)) { Opacity = 0, ShowInTaskbar = false };
        Check(form.ClientSize == new Size(60, 40), "Window fits grid cells");
        form.Karel.Paint(Color.Red);
        form.Karel.Move();
        form.Karel.Paint(Color.Blue);
        form.Karel.Move();
        using var bitmap = new Bitmap(form.Width, form.Height);
        form.DrawToBitmap(bitmap, new Rectangle(Point.Empty, bitmap.Size));
        Point offset = form.PointToScreen(Point.Empty) - (Size)form.Location;
        Check(bitmap.GetPixel(offset.X + 10, offset.Y + 10).ToArgb() == Color.Red.ToArgb(), "Renders stored red cell");
        Check(bitmap.GetPixel(offset.X + 30, offset.Y + 10).ToArgb() == Color.Blue.ToArgb(), "Renders stored blue cell");
        Check(bitmap.GetPixel(offset.X + 10, offset.Y + 30).ToArgb() == Color.White.ToArgb(), "Renders untouched cell");
        using var timer = new System.Windows.Forms.Timer { Interval = 20 };
        int ticks = 0;
        timer.Tick += (_, _) => { if (++ticks == 3) form.Close(); };
        timer.Start();
        Application.Run(form);
        Check(ticks == 3 && form.IsDisposed, "UI processes timers and closes cleanly");
        Console.WriteLine($"Passed {checks} checks.");
    }

    private static void TestDemo()
    {
        bool completed = false;
        bool colorsCorrect = false;
        var elapsed = System.Diagnostics.Stopwatch.StartNew();
        using var observer = new System.Windows.Forms.Timer { Interval = 100 };
        observer.Tick += (_, _) =>
        {
            var form = Application.OpenForms.OfType<KarelUI>().FirstOrDefault();
            if (form is null) return;
            form.Opacity = 0;
            form.ShowInTaskbar = false;
            if (form.Text.EndsWith("demo complete"))
            {
                completed = true;
                Size size = form.Karel.GetGridSize();
                colorsCorrect = true;
                for (int y = 0; y < size.Height; y++)
                    for (int x = 0; x < size.Width; x++)
                        colorsCorrect &= form.Karel.GetColorAt(x, y).ToArgb() == (y % 2 == 0 ? Color.Coral : Color.SkyBlue).ToArgb();
                form.Close();
            }
            else if (elapsed.Elapsed.TotalSeconds > 90) form.Close();
        };
        EventHandler startObserver = (_, _) => observer.Start();
        Application.Idle += startObserver;
        try
        {
            System.Reflection.Assembly.Load("karelvm").EntryPoint!.Invoke(null, [Array.Empty<string>()]);
        }
        finally { Application.Idle -= startObserver; }
        Check(completed, "Actual demo completes its traversal");
        Check(colorsCorrect, "Actual demo paints every cell with the expected color");
        Console.WriteLine("Full demo passed: all 600 cells painted, message loop responsive, window closed cleanly.");
    }
}
