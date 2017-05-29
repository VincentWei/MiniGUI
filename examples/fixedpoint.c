void draw_arc (HDC hdc, POINT* pts)
{
            int sx = pts [0].x, sy = pts [0].y;
            int dx = pts [1].x - sx, dy = pts [1].y - sy;
            int r = sqrt (dx * dx * 1.0 + dy * dy * 1.0);
            double cos_d = dx * 1.0 / r;
            fixed cos_f = ftofix (cos_d);
            fixed ang1 = fixacos (cos_f);
            int r2;
            fixed ang2;

            if (dy > 0) {
                ang1 = fixsub (0, ang1);
            }

            dx = pts [2].x - sx;
            dy = pts [2].y - sy;
            r2 = sqrt (dx * dx * 1.0 + dy * dy * 1.0);
            cos_d = dx * 1.0 / r2;
            cos_f = ftofix (cos_d);
            ang2 = fixacos (cos_f);
            if (dy > 0) {
                ang2 = fixsub (0, ang2);
            }

            Arc (hdc, sx, sy, r, ang1, ang2);
}
