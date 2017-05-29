    /* Draw an arc from 0 radians to 1 radians */
    Arc (hdc, 100, 100, 100, 0, itofix (1));

    /* Draw an arc from 0.5 radians to 1.5 radians */
    fixed radian1 = itofix (1);
    fixed radian2 = itofix (2);
    fixed radian05 = fixdiv (radian1, radian2);
    fixed radian15 = fixdiv (fadd (radian1, randian2), radian2);

    Arc (hdc, 100, 100, 100, radian05, radian15);
