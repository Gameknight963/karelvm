using karel_cpp;

namespace karelui
{
    public partial class KarelUI : Form
    {
        public readonly Karel Karel;
        const int CellSize = 20;

        public KarelUI(Size size)
        {
            Karel = new Karel(size.Width, size.Height);
            InitializeComponent();
            ClientSize = new Size(checked(size.Width * CellSize), checked(size.Height * CellSize));
            DoubleBuffered = true;
            Karel.Changed += (_, _) => Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            int gridX = Karel.GetGridSize().Width;
            int gridY = Karel.GetGridSize().Height;

            for (int y = 0; y < gridY; y++)
            {
                for (int x = 0; x < gridX; x++)
                {
                    using Brush brush = new SolidBrush(Karel.GetColorAt(x, y));
                    e.Graphics.FillRectangle(brush, x * CellSize, y * CellSize, CellSize, CellSize);
                    e.Graphics.DrawRectangle(Pens.LightGray, x * CellSize, y * CellSize, CellSize, CellSize);
                }
            }

            Point position = Karel.GetPosition();
            Point direction = Karel.GetOrientation().AsPoint();
            int centerX = position.X * CellSize + CellSize / 2;
            int centerY = position.Y * CellSize + CellSize / 2;
            Point[] arrow =
            [
                new(centerX + direction.X * 8, centerY + direction.Y * 8),
                new(centerX - direction.X * 5 - direction.Y * 5, centerY - direction.Y * 5 + direction.X * 5),
                new(centerX - direction.X * 5 + direction.Y * 5, centerY - direction.Y * 5 - direction.X * 5)
            ];
            e.Graphics.FillPolygon(Brushes.Black, arrow);
        }
    }
}
