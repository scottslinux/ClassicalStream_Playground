# StreamBox enclosure: a cylinder with a 45 degree top that holds the 2.8" display.
#
# Builds two printable parts and writes STEP (for Shapr3D) and STL (for the slicer):
#   enclosure/streambox_body.step / .stl   - cylinder shell + slanted screen face
#   enclosure/streambox_base.step / .stl   - bottom plate with Feather + amp mounts
#
# Run (needs CadQuery: pip install cadquery):
#   python3 enclosure/enclosure.py
#
# Coordinates: Z is up, base bottom at Z=0. The screen faces the FRONT (-X).
# The back (+X) is the tall side, where the USB-C and speaker cable come out.
#
# >>> Every value marked MEASURE is a typical number for a red 2.8" SPI board
# >>> or the Feather ESP32 V2. Check yours with calipers and change it here.
import math, os
import cadquery as cq

OUT = os.path.dirname(os.path.abspath(__file__))

# ------------------------------------------------------------------ enclosure
R          = 45.0     # outer radius (90 mm cylinder)
WALL       = 2.4      # side wall thickness
H_LOW      = 30.0     # height of the short (front) side
FACE_T     = 3.0      # thickness of the slanted screen face
FACE_LIP   = 1.0      # bezel left in front of the glass
TILT       = 45.0     # screen angle (degrees from horizontal)
BASE_T     = 3.0      # base plate thickness
FIT        = 0.25     # clearance between base plate and wall
TAB_ANGLES = (90, 210, 330)   # screw tabs (degrees, 0 = back), clear of USB and amp

# ------------------------------------------------------------------ display (MEASURE)
PCB_L, PCB_W, PCB_T = 86.0, 50.0, 1.6   # red board size
HOLE_INSET  = 3.0     # screw hole center to board edge
HOLE_D      = 3.2     # board hole size
GLASS_L, GLASS_W = 69.2, 50.0           # LCD glass outline
GLASS_T     = 3.8     # front of PCB to front of glass
GLASS_SHIFT = 0.0     # glass center offset along the board (+ = away from header)
ACTIVE_L, ACTIVE_W = 57.6, 43.2         # visible pixels (240x320 at 0.18 mm)
WINDOW_MARGIN = 0.8   # window bigger than the pixels by this on each side
HEADER_AT_BOTTOM = True   # 14 pin header end at the low/front edge (see notes)
SCREW_PILOT = 2.2     # pilot hole for M2.5 self tapping screws
BOSS_D      = 6.0

# ------------------------------------------------------------------ Feather ESP32 V2 (MEASURE)
FEATHER_L, FEATHER_W = 52.3, 22.9
FEATHER_HOLES_L, FEATHER_HOLES_W = 45.72, 17.78   # Feather standard hole spacing
STANDOFF_H  = 5.0
USB_Z_ABOVE_PCB = 1.6  # USB-C center above top of Feather PCB
USB_SLOT_W, USB_SLOT_H = 13.0, 7.5   # big enough for most plug overmolds

# ------------------------------------------------------------------ MAX98357A breakout (MEASURE)
AMP_L, AMP_W = 19.4, 17.8
AMP_WALL_H = 4.0

# ------------------------------------------------------------------ openings
SPEAKER_HOLE_D   = 5.0     # speaker cable exit
SPEAKER_HOLE_ANG = 35.0    # degrees around from the back (+X)
SPEAKER_HOLE_Z   = 14.0
WAKE_BUTTON_HOLE = False   # set True for a panel pushbutton on the back
WAKE_BUTTON_D    = 7.0
WAKE_BUTTON_Z    = 30.0

# ================================================================== derived
R_IN = R - WALL
tilt = math.radians(TILT)
TALL = H_LOW + 2*R*math.tan(tilt) + 20          # comfortably taller than the top

# the outer screen plane passes through the top of the front edge
U = cq.Vector(math.cos(tilt), 0, math.sin(tilt))      # up the slope (toward back)
N = cq.Vector(-math.sin(tilt), 0, math.cos(tilt))     # out of the screen
CENTER = cq.Vector(0, 0, H_LOW + R*math.tan(tilt))    # face center above the axis

def face_plane(depth=0.0):
    """plane parallel to the screen face, 'depth' mm inside it"""
    return cq.Plane(origin=CENTER - N*depth, xDir=U, normal=N)

def below(depth):
    """huge solid holding everything under the plane at 'depth'"""
    return cq.Workplane(face_plane(depth)).rect(1000, 1000).extrude(-500)

# ---- where the display sits on the face --------------------------------
# The board is longer than the face is "deep", so it is slid toward the front
# until its back corners clear the wall. Work out the allowed range and use the middle.
d_back = FACE_LIP + GLASS_T + PCB_T           # depth of the back of the PCB
def corner_r(u, y, d):
    x = u*math.cos(tilt) + d*math.sin(tilt)
    return math.hypot(x, y)
lim = R_IN - 0.6
lo, hi = None, None
for i in range(0, 4001):
    s = -20 + i*0.01                               # candidate slide (mm, + = toward front)
    ok = all(corner_r(u - s, y, d) <= lim
             for u in (PCB_L/2, -PCB_L/2) for y in (PCB_W/2, -PCB_W/2)
             for d in (FACE_LIP + GLASS_T, d_back))
    if ok:
        lo = s if lo is None else lo
        hi = s
if lo is None:
    raise SystemExit("display board does not fit: make R bigger")
SLIDE = (lo + hi)/2
PCB_U = -SLIDE                                     # board center along the slope

# header at the bottom means the glass offset points up the slope
gdir = 1 if HEADER_AT_BOTTOM else -1
GLASS_U = PCB_U + gdir*GLASS_SHIFT

# ================================================================== body
def build_body():
    outer = cq.Workplane("XY").circle(R).extrude(TALL).intersect(below(0))
    try:   # soften the top edge of the face
        outer = outer.edges(cq.selectors.BoxSelector((-R-1, -R-1, 1), (R+1, R+1, TALL))).fillet(1.5)
    except Exception as e:
        print("  (skipped top edge fillet:", e, ")")

    cavity = (cq.Workplane("XY").workplane(offset=-1).circle(R_IN).extrude(TALL)
              .intersect(below(FACE_T)))
    body = outer.cut(cavity)

    # window for the pixels, all the way through the face
    window = (cq.Workplane(face_plane(-1)).center(GLASS_U, 0)
              .rect(ACTIVE_L + 2*WINDOW_MARGIN, ACTIVE_W + 2*WINDOW_MARGIN)
              .extrude(-(FACE_T + 2)))
    body = body.cut(window)

    # pocket the glass sinks into from behind (leaves FACE_LIP in front)
    pocket = (cq.Workplane(face_plane(FACE_T)).center(GLASS_U, 0)
              .rect(GLASS_L + 0.4, GLASS_W + 0.4).extrude(FACE_T - FACE_LIP))
    body = body.cut(pocket)

    # screw bosses: from the inside of the face down to the front of the PCB
    boss_len = (FACE_LIP + GLASS_T) - FACE_T
    pts = [(PCB_U + sx*(PCB_L/2 - HOLE_INSET), sy*(PCB_W/2 - HOLE_INSET))
           for sx in (1, -1) for sy in (1, -1)]
    if boss_len > 0:
        bosses = (cq.Workplane(face_plane(FACE_T)).pushPoints(pts)
                  .circle(BOSS_D/2).extrude(-boss_len))
        body = body.union(bosses)
    pilots = (cq.Workplane(face_plane(FACE_LIP + GLASS_T + 0.01)).pushPoints(pts)
              .circle(SCREW_PILOT/2).extrude(FACE_T - 0.8 + max(boss_len, 0)))
    body = body.cut(pilots)

    # tabs inside the bottom of the wall that the base screws into
    for ang in TAB_ANGLES:
        a = math.radians(ang)
        cx, cy = (R_IN - 2.5)*math.cos(a), (R_IN - 2.5)*math.sin(a)
        tab = (cq.Workplane("XY").workplane(offset=BASE_T).center(cx, cy)
               .circle(4.5).extrude(9))
        tab = tab.intersect(cq.Workplane("XY").circle(R - 0.5).extrude(TALL))
        body = body.union(tab)
        hole = (cq.Workplane("XY").workplane(offset=BASE_T).center(cx, cy)
                .circle(1.25).extrude(8))           # M3 self tapping pilot
        body = body.cut(hole)

    # USB-C slot on the back, centered on the Feather's port
    usb_z = BASE_T + STANDOFF_H + 1.6 + USB_Z_ABOVE_PCB
    slot = (cq.Workplane("YZ").workplane(offset=R_IN - 3).center(0, usb_z)
            .slot2D(USB_SLOT_W, USB_SLOT_H).extrude(WALL + 6))
    body = body.cut(slot)

    # speaker cable hole
    a = math.radians(SPEAKER_HOLE_ANG)
    spk = (cq.Workplane(cq.Plane(origin=(0, 0, SPEAKER_HOLE_Z),
                                 xDir=(-math.sin(a), math.cos(a), 0),
                                 normal=(math.cos(a), math.sin(a), 0)))
           .workplane(offset=R_IN - 3).circle(SPEAKER_HOLE_D/2).extrude(WALL + 6))
    body = body.cut(spk)

    if WAKE_BUTTON_HOLE:
        btn = (cq.Workplane("YZ").workplane(offset=R_IN - 3).center(0, WAKE_BUTTON_Z)
               .circle(WAKE_BUTTON_D/2).extrude(WALL + 6))
        body = body.cut(btn)
    return body

# ================================================================== base
FEATHER_REAR_X = math.sqrt((R_IN - 0.5)**2 - (FEATHER_W/2)**2)   # back corners just clear the wall
FEATHER_CX = FEATHER_REAR_X - FEATHER_L/2
AMP_CX = FEATHER_CX - FEATHER_L/2 - 4 - AMP_L/2

def build_base():
    base = cq.Workplane("XY").circle(R_IN - FIT).extrude(BASE_T)

    # Feather standoffs
    fpts = [(FEATHER_CX + sx*FEATHER_HOLES_L/2, sy*FEATHER_HOLES_W/2)
            for sx in (1, -1) for sy in (1, -1)]
    stand = (cq.Workplane("XY").workplane(offset=BASE_T).pushPoints(fpts)
             .circle(2.6).extrude(STANDOFF_H))
    base = base.union(stand)
    base = base.cut(cq.Workplane("XY").workplane(offset=BASE_T - 1.5).pushPoints(fpts)
                    .circle(SCREW_PILOT/2).extrude(STANDOFF_H + 2))

    # amp cradle: low walls, open toward the Feather for the wires
    iw, il = AMP_W + 0.6, AMP_L + 0.6
    ring = (cq.Workplane("XY").workplane(offset=BASE_T).center(AMP_CX, 0)
            .rect(il + 3.2, iw + 3.2).rect(il, iw).extrude(AMP_WALL_H))
    gap = (cq.Workplane("XY").workplane(offset=BASE_T).center(AMP_CX + il/2 + 1.6, 0)
           .rect(4, iw - 4).extrude(AMP_WALL_H + 1))
    base = base.union(ring.cut(gap))

    # screw holes to the wall tabs, countersunk from below
    for ang in TAB_ANGLES:
        a = math.radians(ang)
        cx, cy = (R_IN - 2.5)*math.cos(a), (R_IN - 2.5)*math.sin(a)
        base = base.cut(cq.Workplane("XY").workplane(offset=-1).center(cx, cy)
                        .circle(1.7).extrude(BASE_T + 2))
        base = base.cut(cq.Workplane("XY").center(cx, cy)
                        .circle(3.2).workplane(offset=1.7).circle(1.7).loft())

    # vent slots on both sides of the Feather
    for y in (-27, -21, 21, 27):
        base = base.cut(cq.Workplane("XY").workplane(offset=-1).center(FEATHER_CX, y)
                        .slot2D(28, 2.4).extrude(BASE_T + 2))
    return base

# ================================================================== fit check helpers
def display_dummy():
    """PCB + glass + header/dupont keep-out, so we can test for collisions"""
    p = (cq.Workplane(face_plane(FACE_LIP + GLASS_T)).center(PCB_U, 0)
         .rect(PCB_L, PCB_W).extrude(-PCB_T))
    g = (cq.Workplane(face_plane(FACE_LIP)).center(GLASS_U, 0)
         .rect(GLASS_L, GLASS_W).extrude(-GLASS_T))
    hdr_u = PCB_U + (-1 if HEADER_AT_BOTTOM else 1)*(PCB_L/2 - 3)
    h = (cq.Workplane(face_plane(d_back)).center(hdr_u, 0)
         .rect(6, 37).extrude(-23))           # header pins + dupont plugs
    return p.union(g), h

def feather_dummy():
    z0 = BASE_T + STANDOFF_H
    pcb = cq.Workplane("XY").workplane(offset=z0).center(FEATHER_CX, 0).rect(FEATHER_L, FEATHER_W).extrude(1.6)
    parts = cq.Workplane("XY").workplane(offset=z0 + 1.6).center(FEATHER_CX, 0).rect(FEATHER_L - 4, FEATHER_W - 2).extrude(4)
    return pcb.union(parts)

def vol(wp):
    try:
        return wp.val().Volume()
    except Exception:
        return 0.0

if __name__ == "__main__":
    print("display slide toward front: %.1f mm (allowed %.1f .. %.1f)" % (SLIDE, lo, hi))
    body = build_body()
    base = build_base()

    disp, hdr = display_dummy()
    checks = {
        "display vs body":         vol(disp.intersect(body)),
        "header/dupont vs body":   vol(hdr.intersect(body)),
        "header/dupont vs base":   vol(hdr.intersect(base)),
        "feather vs body":         vol(feather_dummy().intersect(body)),
        "header/dupont vs feather":vol(hdr.intersect(feather_dummy())),
        "base vs body":            vol(base.intersect(body)),
    }
    for k, v in checks.items():
        print("  %-26s %s" % (k, "clear" if v < 0.01 else "COLLIDES (%.1f mm3)" % v))

    bb = body.val().BoundingBox()
    print("body size: %.1f x %.1f x %.1f mm (tall side %.1f)" % (bb.xlen, bb.ylen, bb.zlen, bb.zmax))
    for name, part in (("streambox_body", body), ("streambox_base", base)):
        cq.exporters.export(part, os.path.join(OUT, name + ".step"))
        cq.exporters.export(part, os.path.join(OUT, name + ".stl"), tolerance=0.05, angularTolerance=0.1)
    # stand-in blocks for the boards, to import alongside the parts and see the fit
    cq.exporters.export(disp.union(hdr), os.path.join(OUT, "reference_display.step"))
    cq.exporters.export(feather_dummy(), os.path.join(OUT, "reference_feather.step"))
    print("wrote STEP + STL to", OUT)
