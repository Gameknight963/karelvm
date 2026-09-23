using karel_cpp;
using karelui;

namespace karelvm
{
    static class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            ApplicationConfiguration.Initialize();
            using KarelUI form = new(new Size(30, 20));
            using System.Windows.Forms.Timer timer = new() { Interval = 100 };
            int row = 0;
            form.Karel.Paint(Color.Coral);

            form.Text = "demo";

            timer.Tick += (_, _) =>
            {
                Karel karel = form.Karel;
                Point position = karel.GetPosition();
                Size grid = karel.GetGridSize();
                bool headingEast = karel.GetOrientation().GetOrientation() == Orientation.East().GetOrientation();
                bool atEdge = headingEast ? position.X == grid.Width - 1 : position.X == 0;
                if (atEdge)
                {
                    if (position.Y == grid.Height - 1)
                    {
                        timer.Stop();
                        form.Text = "demo complete";
                        return;
                    }
                    if (headingEast) karel.RotateRight(); else karel.RotateLeft();
                    karel.Move();
                    if (headingEast) karel.RotateRight(); else karel.RotateLeft();
                    row++;
                }
                else
                {
                    karel.Move();
                }
                karel.Paint(row % 2 == 0 ? Color.Coral : Color.SkyBlue);
            };
            timer.Start();
            Application.Run(form);
        }
    }
}
