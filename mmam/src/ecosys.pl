#! /usr/bin/perl
use strict;
my $full_eco = 1;
my ($SP, $AB);
$| = 0;

my (
	$output, $ast_list, $gr_spr, $last_pre, $lvl_count, $by_nam,
	%have_sprites, @firsts, %auto_count, $recno, @needspr, $fix_desc_func,
	@auto_mbl, @full_mbl, %has_big, %has_chill, %has_die
);
my @table;
my @chain_typ = qw(LIKES_TO_EAT WILL_EAT HATES SCARED_OF);
my @chain_mark = qw(* . H S);


sub my_list($$) {
        my ($lt, $who) = @_;
	my @list = ();
        foreach my $ln (@table) {
        	if ($ln =~ /$lt\($who\,/) {
           		my $shrt = $ln;
           		$shrt =~ s/^.*?\,//;
           		$shrt =~ s/\).*//;
           		@list = split /\,/, $shrt;
                	last;
           	}
        }
	@list;
}

sub in_list($$) {
        my ($lt, $who) = @_;
	my @list = ();
        foreach my $ln (@table) {
        	if ($ln =~ /$lt\(/) {
           		my $shrt = $ln;
           		if ($shrt =~ /^.*?\((.*?)\,/) {
                                my $inl = $1;
           			$shrt =~ s/^.*?\,/,/;
           			$shrt =~ s/\).*/,/;
           			push @list, $inl if ($shrt =~ /\,$who\,/);
               		}
           	}
        }
	@list;
}

sub show_lists($) {
        my ($who) = @_;
        print "============= $who ===========\n";
        foreach my $e (@chain_typ) {
                my $l = join ", ", my_list($e, $who);
        	print "I $e: " . $l . "\n";
        }
        foreach my $e (@chain_typ) {
                my $l = join ", ", in_list($e, $who);
          	print "$e ME: " . $l . "\n";
        }
        print "\n";
}

sub load_monstbase_cc() {
	open MB, "monstbase.cc" or die "could not open monstbase.cc";
	my @mbln = <MB>;
	foreach (grep /^ *monst_base +[^ ]* *= *.*/, @mbln) {
		/^ *monst_base +([^ ]*)/;
		my $n = $1; $n =~ s/mb_//;
		push @full_mbl, $n;
	}
        my @clean_lines;
        my $slurp = 0;
        my $remark = 0;
        my $glob = "";
        foreach my $ln (@mbln) {
          	$ln =~ s|\x0d?\x0a||g;
        	$ln =~ s|/\*.*?\*/||g;
                $ln =~ s|//.*||;
                my $post_remark = ($ln =~ s|/\*.*||);
                if ($remark) { $remark = !($ln =~ s|.*?\*/||); }
                if (!$remark) {
                        if (!$slurp) {
                        	$slurp = ($ln =~ /^(LIKES_TO_EAT|WILL_EAT|HATES|SCARED_OF)\(/);
                        }
                	if ($slurp) {
                   		$ln =~ s|\&mb_||g;
                                $ln =~ s| ||g;
                                $ln =~ s|\t||g;
                                $ln =~ s|\,\,+|,|g;
                		$glob .= $ln;
                		$glob =~ s|\,\)|)|;
                		if ($ln =~ /\)/) {
                                        push @clean_lines, $glob;
                                        $glob = '';
                                	$slurp = 0;
                    		}
                	}
                }
                $remark = 1 if $post_remark;
        }
        @table = @clean_lines;
        #foreach (@clean_lines) { print "$_\n"; }
	close MB;
}

sub show_full_chain() {
	load_monstbase_cc();
	foreach (@full_mbl) {
		show_lists $_;
	}
}

my ( @mon_base, @plant_base, @city_base );

my %attach_mon = (
 roachy => 90,  pygment => 80, furbit => 230, shruver => 66,
 radrunner => 50, mubark =>40, molber => 300, ackatoo=>90,
 squeeker =>33, meeker=>15, sneeker=>62, vaerix=>66,
 yazzert =>18, malacumebit=>0, squarger=>20, slymer=>34,
 bervul=>40, rexrex=>40, throbid=>90, toothworm=>50,
 krulasite =>9, quitofly => 20, frogert => 40, quiver => 40,
 orick =>0, cheech=>22, hoveler=>30, stalker=>40, crysillian=>0,
 drexler=>80, krakin=>50, dragoonigon=>30, mamothquito=>18,
 uberworm=>25, bigasaurus=>9, titurtilion =>4, golgithan =>0,
);

my @ordered_mon = qw{
 roachy   pygment   furbit   shruver   radrunner
 mubark   molber    ackatoo  squeeker  meeker
 sneeker  vaerix    yazzert  malacumebit squarger
 slymer   bervul    rexrex   throbid   toothworm
 krulasite quitofly frogert  quiver    orick
 cheech   hoveler   stalker  crysillian drexler
 krakin   dragoonigon mamothquito uberworm bigasaurus
 titurtilion golgithan
};

#hunter_camp merchant_camp hippie_camp

my %city_cfg = (
	orick_main_base => 0,
	orick_scout_party => 40,
	orick_small_base => 12,
	orick_war1 => 4,
	orick_war2 => 3,

        raider_base1 => 20,
        raider_base2 => 12,
        raider_base3 => 6,
        raider_base4 => 4,
        raider_ship_fleet => 3,
        raider_ship_fleet2 => 3,
        police_vs_raiders => 10,

	crash_site1 => 2,
	crash_site2 => 2,
	crash_site3 => 2,
	crysillian_hd => 2,
	crysillian_ruins => 13,
	crysillian_ufos => 2,

	stalkplant_farm => 8,

	beholder_hd => 3,
	molber_ranch_chal => 1,
	big_vs_hovelers => 5,

  	rural_town=>22, heavy_city=>16,
  	jmart => 80,
  	bfc => 70,
  	arena => 40,
  	fisec => 15,
  	wisehut => 20,
);


sub ok_forest_idx() {
        my $setc;
        do {
               	$setc = int(rand(999)+1);
        } while ($plant_base[$setc] || $city_base[$setc] =~ /(raider_town|orick_town|orick_war|heavy_city|jmart|bfc|arena|fisec)/);
        return $setc;
}

sub ok_sparce_idx() {
        my $setc;
        do {
               	$setc = int(rand(999)+1);
        } while ($plant_base[$setc] || $city_base[$setc] =~ /heavy_city/);
        return $setc;
}


sub ok_void_idx() {
        my $setc;
        do {
               	$setc = int(rand(999)+1);
        } while ($plant_base[$setc] || $city_base[$setc]);
        return $setc;
}


sub build_fcl() {
        die "cant open ecosys.cc" unless open ECO, ">ecosys.cc";
        die "cant open custom levels dir" unless opendir CLVL, "../data/custom";
        while (defined (my $clvl = <CLVL>)) {
                if ($clvl =~ /\.mmam$/) {
                	$clvl =~ s/\.mmam$//;
        		if ($clvl !~ /^_/ && !defined $city_cfg{$clvl}) {
				$city_cfg{$clvl} = 1;
                                print STDERR "warning: $clvl city type implicitly defined!\n";
           		}
   	        }
        }
        srand(576531);
        $city_base[80] = $city_base[280] = $city_base[568] = $city_base[880] = "orick_main_base";

        foreach my $ctyp (keys %city_cfg) {
          for (1 .. $city_cfg{$ctyp} ) {
                my $setc;
          	do {
                	$setc = int(rand(999)+1);
             	} while ($city_base[$setc]);
             	my $str = $ctyp;
                if ($ctyp eq "heavy_city") {
			$str .= " jmart" if rand(1) < 0.80;
			$str .= " bfc" if rand(1) < 0.80;
			$str .= " fisec" if rand(1) < 0.40;
                } elsif ($ctyp eq "rural_town") {
			$str .= " jmart" if rand(1) < 0.30;
			$str .= " bfc" if rand(1) < 0.35;
			$str .= " fisec" if rand(1) < 0.10;
                }
                $city_base[$setc] = $str;
          }
        }
        $plant_base[ok_forest_idx()] = "forest happytree" for (1 .. 18);
        $plant_base[ok_forest_idx()] = "forest happytree mimmack" for (1 .. 3);
        $plant_base[ok_forest_idx()] = "forest shrub fern" for (1 .. 8);
        $plant_base[ok_void_idx()] = "void" for (1 .. 13);
        for (1 .. 13) { my $ix = ok_void_idx();  $plant_base[$ix] = "dieing"; $mon_base[$ix] = "roachy "; }
        for (1 .. 7) { my $ix = ok_void_idx();  $plant_base[$ix] = "dieing"; $mon_base[$ix] = "toothworm uberworm "; }
        for (1 .. 3) { my $ix = ok_void_idx();  $plant_base[$ix] = "dieing"; $mon_base[$ix] = "dragoonigon "; }
        for (1 .. 2) { my $ix = ok_void_idx();  $plant_base[$ix] = "dieing"; $mon_base[$ix] = "krulasite "; }
        $plant_base[ok_sparce_idx()] = "sparce happytree" for (1 .. 7);
        $plant_base[ok_sparce_idx()] = "sparce psychoshroom" for (1 .. 6);
        $plant_base[ok_sparce_idx()] = "sparce pyroshroom" for (1 .. 8);
        $plant_base[ok_sparce_idx()] = "sparce poisonshroom" for (1 .. 12);
        $plant_base[ok_sparce_idx()] = "sparce psychoshroom pyroshroom poisonshroom" for (1 .. 3);
        $plant_base[ok_sparce_idx()] = "sparce shrub fern pyroshroom" for (1 .. 3);
        $plant_base[ok_sparce_idx()] = "sparce shrub poisonshroom" for (1 .. 3);
        for my $ast_idx (1 .. 999) {
        	$plant_base[$ast_idx] = (
        		$city_base[$ast_idx] =~ /heavy_city/ ? "void" :
                        $ast_idx >= 800 ? "sparce stalkplant" :
                        $ast_idx - rand(500) < 0 ? "sparce fern" :
                        $ast_idx - rand(500) > 0 ? "sparce shrub" : "sparce fern shrub"
        	) if (!$plant_base[$ast_idx]);
   	}
        foreach my $mon (@ordered_mon) {
                my @can_eat = (my_list("LIKES_TO_EAT", $mon), my_list("WILL_EAT", $mon));
                # print STDERR "search $mon [" . join(":", @can_eat) . "]\n";

                for my $pass (1 .. $attach_mon{$mon}) {
                	my $tries = 0; my $max_tries = 1000; my $ok = 0;
        		my ($setc, $bc);
        		$setc = int(rand(999)+1);
        		do {
               			if (++$tries == $max_tries) {
                   			print STDERR "no feeders for $mon (pass $pass) [" . join(":", @can_eat) . "]\n";
                        		last;
                   		}
                        	$bc = " $plant_base[$setc] $mon_base[$setc] ";
                        	if ($bc !~ / $mon / && $city_base[$setc] !~ /(raider|bfc)/ && $plant_base[$setc] !~ /(dieing|void)/) {
                        		foreach my $eat (@can_eat) {
                        			$ok = ($bc =~ / $eat /);
                        			if ($ok) {
                                			$mon_base[$setc] .= "$mon ";last;
                           			}
                        		}
                        	}
                        	$setc++; $setc = 1 if $setc == 1000;
        		} while (!$ok);
                }
        }
        my $ind = "\t\t";
        print ECO "// ecosys.cc (generated by ecosys.pl)  Do not hand edit!\n";
        print ECO "#include \"mmam.h\"\n\n";
        print ECO "void asteroid_t::populate(int idx) {\n";
        print ECO "\tswitch (idx) {\n";
        print ECO "\tcase 0: spawn_monst(&mb_fern, 12); spawn_monst(&mb_shrub, 12); break; // Home Ast\n";
                
	for my $ast_idx (1 .. 999) {
   		print ECO "\tcase $ast_idx: // [$city_base[$ast_idx]] [$plant_base[$ast_idx]] {$mon_base[$ast_idx]}\n";
                if ($full_eco) {
                  	foreach my $cp (split / /, $city_base[$ast_idx]) {
                        	if ($cp =~ /^(jmart|bfc|arena|fisec|wisehut)$/) {
                           		print ECO $ind . "spawn_monst(&mb_$cp);\n";
   		                } elsif ($cp =~ /^(heavy_city|rural_town)$/) {
                       			print ECO $ind . "spawn_city();\n";
                       		} else {
                                        print ECO $ind . "script_load(\"$cp.mmam\", this);\n";
                       		}
                     	}
                        my @plants = (split / /, $plant_base[$ast_idx]);
                        my $gpat = shift @plants;
                        my $num_plants = 0;
                        foreach my $cp (@plants) {
                        	if ($cp =~ /^(mimmack)$/) {
                           		print ECO $ind . "spawn_monst(&mb_$cp);\n";
   		                } else {
                       			my $addc = (
                                        	$cp =~ /shroom/ ? int(rand(12)+8) :
                                        	$gpat eq 'forest' ? (
                                           		$cp eq 'happytree' ? int(rand(32)+60) : int(rand(12)+35)
                                                ) : int( (rand(12)+40) / (1+$#plants))
               				);
               				$num_plants += $addc;
                                        print ECO $ind . "spawn_monst(&mb_$cp, $addc);\n";
                       		}
                     	}
                     	my @monsters = split / /, $mon_base[$ast_idx];
                     	my @food = (@plants, @monsters);
                     	foreach my $cp (@monsters) {
                        	my @will_eat = (my_list("LIKES_TO_EAT", $cp), my_list("WILL_EAT", $cp));
                                my @can_eat = ();
                                foreach my $eat (@will_eat) {
                                	foreach my $f (@food) { push @can_eat, $f if $f eq $eat; }
                                }
                                my $swarm = int(
                                        $cp =~ /^(meeker|krakin|dragoonigon|mamothquito|uberworm|bigasaurus|titurtilion)$/ ? 1 + rand(3) :
                                	$cp eq "roachy" ? ($gpat eq "dieing" ? int(rand(12)+20) : 3 ) :
                                	$cp eq "toothworm" ? ($gpat eq "dieing" ? int(rand(12)+20) : 3 ) :
                                        $cp eq "pygment" ||   $cp eq "furbit" || $cp eq "shruver" ||
                                        $cp eq "radrunner" || $cp eq "frogert" || $cp eq "mubark" ? ($gpat eq "forest" ? 10 : int($num_plants/4.8)+ 1) :
                                        $cp eq "molber" ? ($gpat eq "forest" ? 2 : int(rand(8)) == 0 ? 35 : int($num_plants/6.2) + 1) :
                                        $cp eq "ackatoo" ? ($gpat eq "forest" ? $num_plants / 3 + 1 : $num_plants/8 + 1) :
 					$cp eq "slymer" ? ($gpat ne "forest" && int(rand(8)) == 0 ? 35 : 7) :
                                	$cp eq "throbid" ? int(rand(5)+3) :
                                        $cp eq "drexler" ? int(rand(3)+4) :
 					$cp eq "rexrex" ? 2 + rand(3) :
	        			$cp eq "stalker" ?  6 :
	        			$cp eq "krulasite" ? 6 :
	        			$cp eq "cheech" ? 4 :
	        			$cp eq "hoveler" ? 8 :
 					$cp eq "squeeker" ? 3 :
 					$cp eq "sneeker" ? 9 :
                                        $cp eq "quiver" ? 6 : 					
 					$cp eq "vaerix" ? 14 :
 					$cp eq "yazzert" ? 6 :
 					$cp eq "squarger" ? 6 :
 					$cp eq "bervul" ? 7 :
 					$cp eq "quitofly" ? 10 : "XXX"
                                );
                           	print ECO $ind . "spawn_monst(&mb_$cp, $swarm); //eats: " . join(", ",@can_eat) . "\n";
                           	print STDERR "no spawn rule $cp\n" if ($swarm eq "XXX" || $swarm <= 0);
                     	}
                     	print ECO $ind . "break;\n";
                }
   	}
	print ECO "\t}\n}\n";
	close ECO;
	`dat levels.dat -c1 -a -t DATA ../data/custom/*.mmam`;
}

load_monstbase_cc();
build_fcl();
# show_full_chain();
